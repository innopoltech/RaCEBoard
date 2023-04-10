#include "VirtualDevice.h"

VirtualDev_ VirtualDev;


bool Barometer_m::begin()
{
  init = true;
}

double Barometer_m::GetHeight()
{
  if(!init)
    return 0;
    
  uint8_t data[4];
  float temp;

  if(!VirtualDev.FlagBar)
    VirtualDev.SendCMD(BARO_GetHeight);
  VirtualDev.FlagBar = false;
  VirtualDev.ExtractData(data,25,4);

  memcpy(&temp, data, 4);
  return (double)temp - ZeroHeight;
}

void Barometer_m::SetZeroHeight()
{
  ZeroHeight=GetHeight();
}


/////////////////////////////////////////////////////////////////

bool IMU_m::begin()
{
  init = true;
}
bool IMU_m::GetAccel(double * bf_)
{
  if(!init)
    return false;

  uint8_t data[12];
  float bf_f[3];


  if(!VirtualDev.FlagAccel)
     VirtualDev.SendCMD(IMU_GetAccel);
  VirtualDev.FlagAccel = false;
 
  VirtualDev.ExtractData(data,1, 12);
  memcpy(bf_f, data, 12);

  for(uint8_t i=0;i<3;i++)
    bf_[i] = (double)bf_f[i];

  return true;
}
bool IMU_m::GetGyro(double * bf_)
{
  if(!init)
    return false;

  uint8_t data[12];
  float bf_f[3];

  if(!VirtualDev.FlagGyro)
     VirtualDev.SendCMD(IMU_GetGyro);
  VirtualDev.FlagGyro = false;
  
  VirtualDev.ExtractData(data,13, 12);

  memcpy(bf_f, data, 12);
  for(uint8_t i=0;i<3;i++)
    bf_[i] = (double)bf_f[i];

  return true;
}

/////////////////////////////////////////////////////////////////


uint8_t GPIO_m::digitalRead_jump()
{
  uint8_t data[1];

  if(!VirtualDev.FlagJumper)
     VirtualDev.SendCMD(JUMPER_digitalRead_jump);
  VirtualDev.FlagJumper = false;
  
  VirtualDev.ExtractData(data,0, 1);


  return data[0];
}

void GPIO_m::digitalWrite_prsht()
{
  VirtualDev.SendCMD(JUMPER_digitalWrite_prsht);
}
