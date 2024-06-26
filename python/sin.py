import math
import time
import com
import sys


if len(sys.argv)!=6:
    print(sys.argv[0]," leg actuator start_pos end_pos speed")
    print("  leg: [1;4]")
    print("  actuator: [1;3]")
    print("  start_pos: [0.0,end_pos[")
    print("  end_pos: ]start_pos;0.2]")
    print("  speed: time in sec to make a cycle (full move from start_pos to end_pos to start_pos)")


leg=int(sys.argv[1])
actuator=int(sys.argv[2])
start_pos=float(sys.argv[3])
end_pos=float(sys.argv[4])
speed=float(sys.argv[5])

if start_pos<0 or start_pos>=end_pos or end_pos>0.2:
    print("condition 0<=start_pos<end_pos<=0.2 not ok")
    sys.exit(1)

center=(start_pos+end_pos)/2.0
magnitude=(end_pos-start_pos)/2.0




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
        v=math.sin((t-t0)/speed*2*math.pi)*magnitude + center
#        print(t-t0,v)
        
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
