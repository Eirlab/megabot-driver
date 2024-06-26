import math
import time
import com
import sys


if len(sys.argv)<2:
    print(sys.argv[0]," leg;actuator;start_pos;end_pos;speed [leg;actuator;start_pos;end_pos;speed]*")
    print("  leg: [1;4]")
    print("  actuator: [1;3]")
    print("  start_pos: [0.0,end_pos[")
    print("  end_pos: ]start_pos;0.2]")
    print("  speed: time in sec to make a cycle (full move from start_pos to end_pos to start_pos)")
    sys.exit(1)

moves=[]
    
for a in sys.argv[1:]:
    s=a.split(";")
    if len(s)!=5:
        print("wrong arg: ",a,s)
        sys.exit(2)
    m={'leg':int(s[0]),'actuator':int(s[1]),'start':float(s[2]),'end':float(s[3]),'speed':float(s[4])}
    if m['start']<0 or m['start']>=m['end'] or m['end']>0.2:
        print("condition 0<=start_pos<end_pos<=0.2 not ok for ",a)
        sys.exit(1)
    m['center']=(m['start']+m['end'])/2.0
    m['amplitude']=(m['end']-m['start'])/2.0
    moves.append(m)





print("initialize controler...")
c=com.ControlerHandler() 
#c.print_string_msg(True)
time.sleep(1)
c.send_info(0.1) # ask for update of status from controler every 100ms
t0=time.time()
print("start move: use CRTL^C to stop")
try:
    while True:
        t=time.time()
        for m in moves:        
            v=math.sin((t-t0)/m['speed']*2*math.pi)*m['amplitude'] + m['center']
            c.send_move(m['leg'],m['actuator'],v,1.0)
            print(m['leg'],m['actuator'],"%.3f %.3f"%(t-t0,v))
        
#        c.send_move(1,1,v,1.0)
#        c.send_move(2,1,v,1.0)      
#        c.send_move(3,1,v,1.0)       
#        c.send_move(4,1,v,1.0)
#        time.sleep(0.05)
#       
        time.sleep(0.02)
except:
    pass
c.stop()
c.join()
