#include "SDCustom.h"

#define MISO_pin 50
#define MOSI_pin 51
#define CLK_pin 52
#define CS_pin 37 

#define LogFrecuency 2// 1Hz-30Hz


bool SDCard_m::begin(String msg_)
{
  pinMode(53,OUTPUT); digitalWrite(53,HIGH);
  pinMode(9,OUTPUT);  digitalWrite(9,HIGH);
  pinMode(10,OUTPUT); digitalWrite(10,HIGH);
  
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  bool Init=SD.begin(CS_pin);
  if(!Init)return false;

  String Name = "Log";
  String EndName = ".txt";
  for (size_t i = 0; i < 96; i++)
  {
    if(!SD.exists(Name+i+EndName))
    {
      LogFile = SD.open(Name+i+EndName, O_READ | O_WRITE | O_CREAT);
      break;
    }
    if(i==95)
    {
      LogFile = SD.open(Name+95+EndName, FILE_WRITE);
      break;
    }
  }
  LogFile.println(msg_);
  FileOk=true;

  return FileOk;
}

void SDCard_m::print(String msg_)
{
  if(FileOk){LogFile.print(msg_);}
}
void SDCard_m::println(String msg_)
{
  if(FileOk){LogFile.println(msg_);}
}

void SDCard_m::end()
{
    LogFile.close();
    FileOk=false;
    SD.end();
}



void SDCard_m::upd()
{
 if(FileOk){LogFile.flush();}
}


