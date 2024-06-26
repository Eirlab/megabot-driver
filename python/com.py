from threading import Thread, Lock

import os
import os.path
import time
import traceback
from serial import Serial, SerialException
import sys
import re
import com_data

from subprocess import Popen,PIPE
import select



class ControlerHandler(Thread):
    def __init__(self,logfile=None):
        Thread.__init__(self)
        self.serials=[]
        if logfile!=None:
            self.logfile=open(logfile,'w')
        else:
            self.logfile=None
        for i in range(10):
            pname="/dev/ttyACM%d"%i
            print("try ",pname)
            if os.path.exists(pname)==False:
                continue
            try:
                port=Serial(port=pname, baudrate=2000000, timeout=0.01)
                print('port ',pname,' opened')
            except Exception as e:
                traceback.print_exc()
                continue
            c=0
            while port.isOpen()==False and c<10:
                time.sleep(0.5)
                c+=1
            if c==10:
                port.close()
                continue
            self.serials.append(port)
        print(self.serials)
        self.print_str_msg=False
        self.shouldStop=False
        self.start_time=time.time()
        self.legs=[]
        for a in [1,2,3,4]:
            self.legs.append([{'enabled':False,'position':0,'target':0,'pwm':0,'pwmtarget':0,'status':-1,'last_order':0,'time':0},
                              {'enabled':False,'position':0,'target':0,'pwm':0,'pwmtarget':0,'status':-1,'last_order':0,'time':0},
                              {'enabled':False,'position':0,'target':0,'pwm':0,'pwmtarget':0,'status':-1,'last_order':0,'time':0}])
        self.start()
    def stop(self):
        self.shouldStop=True

    def send_move(self,leg,actuator,position,pwm=1.0):
        """ leg should be 1,2,3 or 4
        actuator 1,2 or 3
        position is in m
        pwm is used to limit maximal pwm value is between 0 (no move) and 1.0 (max)
        """
        o=com_data.Order()
        o.move(leg,actuator,position,pwm)
        data=o.pack()
        for s in self.serials:
            s.write(data)

    def send_pid(self,leg,actuator,kp,ki,kd):
        o=com_data.Order()
        o.pid(leg,actuator,kp,ki,kd)
        data=o.pack()
        for s in self.serials:
            s.write(data)

    def send_stop(self):
        o=com_data.Order()
        o.stop()
        data=o.pack()
        for s in self.serials:
            s.write(data)

    def send_info(self,freq):
        o=com_data.Order()
        o.print(freq)
        data=o.pack()
        for s in self.serials:
            s.write(data)

    def print_string_msg(self,b):
        self.print_str_msg=b
    def startlog(self,filename):
        if self.logfile!=None:
            self.logfile.close()
            self.logfile=None
        if filename!=None:
            self.logfile=open(filename,'w')
    def print_status(self):
        for l in range(4):
            for a in range(3):
                if self.legs[l][a]['status']!=255:
                    print('L',l+1,a+1,'[status=',self.legs[l][a]['status'],
                          ',pos=',self.legs[l][a]['position'],
                          ',target=',self.legs[l][a]['target'],
                          ',pwm=',self.legs[l][a]['pwm'],
                          ',pwmtarget=',self.legs[l][a]['pwmtarget'],']')
    
    def run(self):
        rawfile=open("raw.log","wb")
        while self.shouldStop==False:
            action=False
            for s in self.serials:
                if s.in_waiting:
                    action=True
                    d=s.read(5000)
                    if len(d)>0:
                        rawfile.write(d)
                    #print("read ",len(d)," bytes")
                    p=0
                    while p<len(d):
                        if d[p]==0xA5 and (len(d)-p)>=com_data.Infos.size:
                            i=com_data.Infos(d[p:])
                            if i.check():
                                #print(".",end='')
                                for a in range(3):
                                    self.legs[i.leg-1][a]['enabled']=True
                                    self.legs[i.leg-1][a]['position']=i.actuators[a].position/10000.0
                                    self.legs[i.leg-1][a]['target']=i.actuators[a].target/10000.0
                                    self.legs[i.leg-1][a]['pwm']=i.actuators[a].pwm/10000.0
                                    self.legs[i.leg-1][a]['pwmtarget']=i.actuators[a].pwmtarget/10000.0
                                    self.legs[i.leg-1][a]['status']=i.actuators[a].status
                                    self.legs[i.leg-1][a]['time']=time.time()-self.start_time
                                    if self.logfile!=None:
                                        try:
                                            self.logfile.write("%d;%d;%d;%.5f;%.5f;%.5f;%.5f;%.5f\n"%(i.leg,a+1,self.legs[i.leg-1][a]['status'],
                                                                        self.legs[i.leg-1][a]['position'],
                                                                        self.legs[i.leg-1][a]['target'],
                                                                        self.legs[i.leg-1][a]['pwm'],
                                                                        self.legs[i.leg-1][a]['pwmtarget'],
                                                                        self.legs[i.leg-1][a]['time']))
                                        except:
                                            pass
                                #print("receive infos...:")
                                #self.print_status()
                                p+=com_data.Infos.size                                
                            else:
                                print("infos check wrong:",i,"\np=",p,"\nlen(d)=",len(d),"\n\n")
                                p+=1
                        elif d[p]==com_data.GlobalInfos.headerValue() and (len(d)-p)>=com_data.GlobalInfos.size:
                            i=com_data.GlobalInfos(d[p:])
                            if i.check():
                                print("stats: main:",end='')
                                print(i.main_ticking,end='')
                                for l in range(4):
                                    for a in range(3):
                                        print('L',l+1,a+1,'[',i.actuator_ticking[l*3+a],',',i.read_ticking[l*3+a],']',end='')
                                print()
                                p+=com_data.GlobalInfos.size
                            else:
                                print("\n\nmain stats check wrong:",i,"\np=",p,"\nlen(d)=",len(d),"\n",d[p:],"\n\n")
                                p+=1
                        else:
                            if self.print_str_msg:
                                x=chr(d[p])
                                if x in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,-;#()=+*0123456789 .\n':
                                    print(x,end='')
                                #else:
                                    #print("\n\n",p,"/",len(d)," %x "%(ord(x)),end='')
                            p+=1
            if action==False:
                time.sleep(0.0001)
        if self.logfile!=None:
            self.logfile.close()
        rawfile.close()
        print("controler thread leaving")
        


if __name__ == "__main__":
    try:
        import sys
        from math import pi, sin
        import readline
        import atexit
        if len(sys.argv)<2:
            c=ControlerHandler()
        else:
            c=ControlerHandler(sys.argv[1])
        ## readline stuff
        histfile = ".history"
        try:    
            readline.read_history_file(histfile)
            # default history len is -1 (infinite), which may grow unruly
            readline.set_history_length(1000)
        except FileNotFoundError:
            pass
        atexit.register(readline.write_history_file, histfile)
        ## end readline stuff
        c.print_string_msg(True)
        time.sleep(0.2)
        c.send_info(0.01)
        order=""
        while order!="q":
            cmd=order.split()
            try:
                if len(cmd)==0:
                    pass
                elif cmd[0]=='s':
                    c.send_stop()
                elif cmd[0]=='l':
                    c.startlog(cmd[1])
                elif cmd[0]=='i':
                    c.send_info(float(cmd[1]))
                elif cmd[0]=='p':
                    c.send_pid(int(cmd[1]),int(cmd[2]),float(cmd[3]),float(cmd[4]),float(cmd[5]))
                elif cmd[0]=='m':
                    c.send_move(int(cmd[1]),int(cmd[2]),float(cmd[3]),float(cmd[4]))
                elif cmd[0]=='sin':
                    leg=int(cmd[1])
                    actu=int(cmd[2])
                    period=float(cmd[3])  # in s
                    maxp=float(cmd[4]) # max pwm
                    print("send actuator to 0.1...")
                    c.send_move(leg,actu,0.1,0.3)
                    time.sleep(3)
                    t=time.time()
                    dt=0
                    c.startlog("sinus.data")
                    while dt<2*period:
                        p=(sin(dt/period*2*pi)+1.0)/2.0 *0.1 + 0.05
                        c.send_move(leg,actu,p,maxp)
                        time.sleep(0.002)
                        dt=time.time()-t
                    c.startlog(None)
            except Exception as e:
                print("cmd exception:",e, "cmd:",cmd)
                pass
            order=input("? ")
            order=order.strip()
        c.send_stop()
    except Exception as e:
        print("Exception:",e)
        pass
    c.stop()
    c.join()
#print("run controler handlers")
#controler=ControlerHandler()
#controler.start()
#print("done")
