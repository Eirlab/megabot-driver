#pragma once

const unsigned char CRC7_POLY = 0x91;

inline __attribute__((always_inline)) void update_crc(unsigned char &crc) {
  for (int j = 0; j < 8; j++) {
    if (crc & 1)
      crc ^= CRC7_POLY;
    crc >>= 1;
  }
}

inline __attribute__((always_inline)) unsigned char get_crc(unsigned char v1,
                                                            unsigned char v2) {
  unsigned char crc = 0;
  crc ^= v1;
  update_crc(crc);
  crc ^= v2;
  update_crc(crc);
  return crc;
}

inline __attribute__((always_inline))  unsigned char get_crc(unsigned char *data,int len) {
  unsigned char crc = 0;
  for(int i=0;i<len;++i){
    crc ^= data[i];
    update_crc(crc);
  }
  return crc;
}
