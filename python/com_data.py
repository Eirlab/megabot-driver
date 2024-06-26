import cstruct

CRC7_POLY = 0x91

def update_crc(crc):
    for j in range(8):
      if crc & 1:
        crc ^= CRC7_POLY
      crc >>= 1
    return crc
        
def get_crc(barray):
    crc=0
    for b in barray:
        crc = crc ^ b
        crc=update_crc(crc)
    return crc

    
class Order(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    
    __def__ = """
        #define ORDER_HEADER 0x13
        #define MOVE 1
        #define STOP 2
        #define PRINT 4
        #define PID 8

        struct {
            unsigned char head;
            unsigned char order;
            unsigned char leg;
            unsigned char actuator;
            unsigned short v1;
            unsigned short v2;
            unsigned short v3;
            unsigned char crc1;
            unsigned char crc2;
        }
    """
    def headerValue():
        return cstruct.getdef('ORDER_HEADER')
    def update_crc(self):
        data=self.pack()
        self.crc1=get_crc(data[:-2])
        data=self.pack()
        self.crc2=get_crc(data[:-1])
        
    def pid(self,leg,actuator,kp,ki,kd):
        self.head=cstruct.getdef('ORDER_HEADER')
        self.order=cstruct.getdef('PID')
        self.leg=leg
        self.actuator=actuator
        self.v1 = int(kp * 100)
        self.v2 = int(ki * 100)
        self.v3 = int(kd * 100)
        self.update_crc()
    def move(self,leg,actuator,position,pwm=1.0):
        self.head=cstruct.getdef('ORDER_HEADER')
        self.order=cstruct.getdef('MOVE')
        self.leg=leg
        self.actuator=actuator
        self.v1 = int(position * 10000)
        self.v2=int(pwm*10000)
        self.v3=0
        self.update_crc()
    def stop(self):
        self.head=cstruct.getdef('ORDER_HEADER')
        self.order=cstruct.getdef('STOP')
        self.update_crc()
    def print(self,freq):
        self.head=cstruct.getdef('ORDER_HEADER')
        self.order=cstruct.getdef('PRINT')
        self.v1 = int(freq*10000)
        self.update_crc()
        


class Infos(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    
    __def__ = """
        #define INFO_HEADER 0xA5

        struct Info{
           unsigned char head;
           unsigned char leg;
           struct {
               unsigned short status;
               short position;
               short target;
               short pwm;
               short pwmtarget;
           } actuators[3];
           unsigned char crc1;
           unsigned char crc2;
        };
    """
    def headerValue():
        return cstruct.getdef('INFO_HEADER')
    def check(self):
        try:
            d=self.pack()
            return self.crc1 == get_crc(d[:-2]) and self.crc2==get_crc(d[:-1])
        except:
            pass
        return False
    def get_position(self,a):
        return self.position/10000.0
    def get_target_position(self,a):
        return self.target / 10000.0
    def get_pwm(self,a):
        return self.pwm / 10000.0
    def get_target_pwm(self,a):
        return self.pwmtarget / 10000.0

class GlobalInfos(cstruct.MemCStruct):
    __byte_order__ = cstruct.LITTLE_ENDIAN
    
    __def__ = """
    #define GLOBAL_INFO_HEADER 0xD1

    struct GlobalInfo {
      unsigned char head;
      unsigned char padding[3];
      unsigned int main_ticking;
      unsigned int actuator_ticking[12];
      unsigned int read_ticking[12];
      unsigned char end_padding[2];
      unsigned char crc1;
      unsigned char crc2;
    };
    """
    def headerValue():
        return cstruct.getdef('GLOBAL_INFO_HEADER')
    def check(self):
        try:
            d=self.pack()
            return self.crc1 == get_crc(d[:-2]) and self.crc2==get_crc(d[:-1]) and self.padding[0]==0 and self.padding[1]==1 and  self.padding[2]==2
        except:
            pass
        return False
    
# o = Order()
# import os
# for l in range(1,4):
#     for v in range(0,200,10):
#         o.move(l,2,v/1000.0,1.0)
#         os.write(1,o.pack())


# i=Infos()
# i.actuators[0].status=0


if __name__ == "__main__":
    g=GlobalInfos()
    print(g.size)
