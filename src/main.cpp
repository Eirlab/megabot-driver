#include "BufferedSerial.h"
#include "PinNameAliases.h"
#include "PinNames.h"
#include "mbed.h"
#include <cstdint>

BufferedSerial pc(USBTX, NC, 115200); // tx, rx
BufferedSerial nanoSerial(NC, D2, 115200); // tx, rx

char buffer[500];

struct nanoValue{
  uint32_t value:10;
  uint32_t actuatorId:2;
  uint32_t legId:3;
  uint32_t padding:1;
  uint32_t check:8;
  uint32_t header:8;
};

const unsigned char CRC7_POLY = 0x91;

void update_crc(unsigned char &crc){
    for (int j = 0; j < 8; j++)
    {
      if (crc & 1)
        crc ^= CRC7_POLY;
      crc >>= 1;
    }
}
 
unsigned char get_crc(unsigned char v1, unsigned char v2)
{
  unsigned char  crc = 0; 
  crc ^= v1;
  update_crc(crc);  
  crc ^= v2;
  update_crc(crc);  
  return crc;
}


bool check_nano(uint32_t value, int &legId, int &laId, int &outvalue){
  struct nanoValue *v=(struct nanoValue *)&value;
  if (v->header==0x55){
    if (v->check == get_crc(value&0xFF,(value>>8)&0xFF)){
      legId=v->legId;
      laId=v->actuatorId;
      outvalue=v->value;
      return true;
    }
  }
  return false;  
}

// main() runs in its own thread in the OS
int main() {
  uint32_t v;
  unsigned char c;

  
  while (true) {
    if (nanoSerial.readable()) {
      nanoSerial.read(&c, sizeof(c));
      
      v=(v>>8) | (c<<24);
      
      //      int l = snprintf(buffer, 500, "read data %x => %x\n", c , v );
      //pc.write(buffer, l);
      int leg,la,value;
      if (check_nano(v,leg,la,value)){
	int l = snprintf(buffer, 500, "new value: %d %d  => %d\n", leg , la,value );
	pc.write(buffer, l);
      }

    }  
    //ThisThread::sleep_for(100ms);
  }
  return 0;
}




