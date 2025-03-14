#include "NanoCommunication.h"
#include <cstdint>

/* Following structure map nano 32bits data */
struct nanoValue
{
  uint32_t value : 10;
  uint32_t actuatorId : 2;
  uint32_t legId : 3;
  uint32_t pbit : 1;
  uint32_t check : 8;
  uint32_t header : 8;
};

/* Check that value is ok (parity bit and CRC ok), if so: update leg,actu and
   mesure and return true else return false
*/
bool NanoDirectCom::check_nano(uint32_t value, int &legSelected, int &linearActuatorSelected,
                               uint16_t &mesureInt)
{
  linearActuatorSelected = -1;
  legSelected = -1;
  struct nanoValue *v = (struct nanoValue *)&value;
  if (v->header == 0x9b)
  {
    uint32_t pbit = v->value ^ (v->value >> 1);
    pbit = pbit ^ (pbit >> 2);
    pbit = pbit ^ (pbit >> 4);
    pbit = pbit ^ (pbit >> 8);
    pbit = pbit & 0x1;
    if ((v->pbit == pbit) &&
        (v->check == get_crc(value & 0xFF, (value >> 8) & 0xFF)))
    {
      if ((v->legId >= 1) && (v->legId <= 4))
        legSelected = v->legId;
      else
      {
        dataError += 1;
        return false;
      }
      if ((v->actuatorId >= 1) && (v->actuatorId <= 3))
        linearActuatorSelected = v->actuatorId;
      else
        return false;
      mesureInt = v->value;
      valid += 1;
      return true;
    }
    else
    {
      if (v->pbit != pbit)
        parityError += 1;
      if (v->check != get_crc(value & 0xFF, (value >> 8) & 0xFF))
        crcError += 1;
    }
  }
  else
  {
    dataError += 1;
  }
  return false;
}

void NanoDirectCom::resetStats()
{
  dataError = 0;
  parityError = 0;
  crcError = 0;
  valid = 0;
}

NanoDirectCom::NanoDirectCom(HardwareSerial *serial, NanoComCallback *next)
{
  this->nextCb = next;
  rxChannel = serial;
  rxChannel->setTimeout(0);
  bufferSize = 500;
  buffer = new char[bufferSize];
}

const char *NanoDirectCom::name() const
{
  return "nano com";
}

void NanoDirectCom::tick()
{
  int linearActuatorSelected;
  int legSelected;
  uint16_t mesureInt;
  unsigned char c = 0;
  // auto t = std::chrono::high_resolution_clock::now();
  int validRead = 0;
  int available = 0;
  // Serial.printf("data available for %p : %d \n",rxChannel,rxChannel->available());
  if ((available = rxChannel->available()) > 0)
  {
    if (available > bufferSize)
      available = bufferSize;
    int r = rxChannel->readBytes(buffer, available);
    // Serial.printf("read %d data... on %p\n",r,rxChannel);
    for (int l = 0; l < r;)
    {
      c = buffer[l];
      v = (v >> 8) | (c << 24);
      if (c == 0x9b)
      {
        if (check_nano(v, legSelected, linearActuatorSelected, mesureInt))
        {
          //          auto t = std::chrono::high_resolution_clock::now();
          //  print("%d;%d;%lf;%d\n", legSelected, linearActuatorSelected,
          //        std::chrono::duration<double>(t - startTime).count(),
          //        mesureInt);
          if (nextCb != nullptr)
            nextCb->value(legSelected, linearActuatorSelected, mesureInt);
          // rxChannel->sync();
          l += 4;
          v = 0;
          validRead += 1;
          if (validRead >= 2)
            return;
        }
        else
        {
          l += 1;
          // print("read on chan B check wrong ! %x => %x\n", cB, vB);
        }
      }
      else
      {
        l += 1;
      }
    }
  }
}
