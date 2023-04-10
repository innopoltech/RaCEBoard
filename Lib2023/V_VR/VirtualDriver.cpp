#include "VirtualDevice.h"

void VirtualDev_::SerialClear()
{
  while(Serial.available())
    Serial.read(); 
}
bool VirtualDev_::SerialWait(uint16_t x)
{
  if(x<=0)
    return false;
    
  while(Serial.available() != 32 && x!=0 )
  {
    delay(1);
    x--;
  }
  return Serial.available() >= 32;
}


bool VirtualDev_::SendCMD(Codes cmd_)
{
  if(!init)
  {
    init = true;
    Serial.begin(115200);
  }
  
  SerialClear();
  memset(msg_tx,0,32);
  //Forming and send msg VanCol

  msg_tx[0] = cmd_;
  msg_tx[30] = 0;
  msg_tx[31] = CRC8();

  Serial.write(msg_tx,32);
  
  
  if(ReciveCMD())
  {
    FlagJumper = true;
    FlagAccel = true;
    FlagGyro = true;
    FlagBar = true;
  }
}

bool VirtualDev_::ReciveCMD()
{
  bool ok = SerialWait(1250);

  if(!ok)
    return false;

  //memset(msg_rx,0,32);
  uint8_t i = 0;
  while(Serial.available() && i<32)
  {
    msg_rx[i++]=Serial.read();
  }

  
  return true;

   

   
}


void VirtualDev_::ExtractData(uint8_t* ptr,uint8_t offset ,uint8_t len)
{
  memcpy(ptr, &msg_rx[offset] ,len);
}

uint8_t VirtualDev_::CRC8()
{
    uint8_t crc=0;
    for(uint8_t i=0;i<31;i++)
    {
        crc+=msg_tx[i];
    }
    return crc;
}
