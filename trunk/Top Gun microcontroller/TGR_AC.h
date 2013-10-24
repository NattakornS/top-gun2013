
  unsigned short crc16(uint8_t *data_p, unsigned short length);
  
//----------------------------------------------------------------------------
//                           Air Controller CRC Function
//----------------------------------------------------------------------------        
    #define POLY 0xA001

    uint16_t CRC16;

    unsigned short crc16(uint8_t *data_p, unsigned short length)
    {
      unsigned char i;
      unsigned int data;
      unsigned int crc = 0xffff;

      if (length == 0)
            return (~crc);

      do
      {
            for (i=0, data=(unsigned int)0xff & *data_p++;
                 i < 8; 
                 i++, data >>= 1)
            {
                  if ((crc & 0x0001) ^ (data & 0x0001))
                        crc = (crc >> 1) ^ POLY;
                  else  crc >>= 1;
            }
      } while (--length);

      //crc = ~crc;
      //data = crc;
      //crc = (crc << 8) | (data >> 8 & 0xff);

      return (crc);
}