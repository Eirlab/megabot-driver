import math
import time
import com

t=0

c=com.ControlerHandler("sin.log")
time.sleep(1)
c.send_info(0.005)
try:
    while True:
        t=time.time()
        v=((math.sin(t/1.0)+1.0)/2.0) *0.1+0.05
        print(v)
        c.send_move(4,3,v)
        time.sleep(0.01)
        c.send_move(4,1,v,0.3)
except:
    pass
c.stop()
c.join()
