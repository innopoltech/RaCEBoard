#include "Telemetry_promini.h"

uint32_t packet = 0;

uint8_t Telemetry_m::on()
{
  packet = 0;

pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);

  LoRa.setSPIFrequency(1E6);
  initialT=LoRa.begin(433E6);
  LoRa.enableCrc();
  LoRa.setSignalBandwidth(500E3);
  LoRa.setSpreadingFactor(12);
  LoRa.setGain(6);
  return initialT;
}

void Telemetry_m::off()
{
  LoRa.end();
  initialT=0;
}

void Telemetry_m::SetLowPower()
{
  LoRa.setTxPower(4);
}

void Telemetry_m::SetMaxPower()
{
  LoRa.setTxPower(20);
}

void Telemetry_m::SetChannel(Channels ch_)
{
  long freq_=433E6;
  int sw_=0x12;

  switch (ch_)
  {
  case Channel_1:{freq_=437700E3;sw_=0x13;}
    break;
  case Channel_2:{freq_=437800E3;sw_=0x14;}
    break;
  case Channel_3:{freq_=437900E3;sw_=0x15;}
    break;
  case Channel_4:{freq_=438000E3;sw_=0x16;}
    break;
  case Channel_5:{freq_=438100E3;sw_=0x17;}
    break;
  case Channel_6:{freq_=438200E3;sw_=0x18;}
    break;
  default:
    break;
  }
  LoRa.setFrequency(freq_);
  LoRa.setSyncWord(sw_);

}




/////////////////////////////////////////////////////////////Send
void Telemetry_m::SendS(String msg_)
{
  LoRa.beginPacket();
  LoRa.print(msg_);
  LoRa.endPacket();
}
void Telemetry_m::SendIntDouble(int32_t *msg_,uint8_t len_i, double* msg__,uint8_t len_d)
{
  uint8_t a=sizeof(int32_t);
  uint8_t b=sizeof(double);

  uint8_t buf[a];
  uint8_t buff_d[b];
  LoRa.beginPacket();
  for (size_t k = 0; k < len_i; k++)
  {
    memcpy(buf,&msg_[k],a);
    for (uint8_t i = 0; i < a; i++)
    {
      LoRa.write(buf[i]);
    }
  }
  for (size_t k = 0; k < len_d; k++)
  {
    memcpy(buff_d,&msg__[k],b);
    for (uint8_t i = 0; i < b; i++)
    {
      LoRa.write(buff_d[i]);
    }
  }
  LoRa.endPacket();
}

void Telemetry_m::AsyncOn()
{
  while ( (initialT) && LoRa.beginPacket() == 0) {delay(10);}
}

bool Telemetry_m::AsyncSendS(String msg_)
{
  if(LoRa.beginPacket() == 0)return false;
  LoRa.beginPacket();
  LoRa.print(msg_);
  LoRa.endPacket(true);
  return true;
}

bool Telemetry_m::AsyncSendIntDouble(int32_t *msg_,uint8_t len_i, double* msg__,uint8_t len_d)
{
  if(LoRa.beginPacket() == 0)return false;
  
  uint8_t a=sizeof(int32_t);
  uint8_t b=sizeof(double);

  uint8_t buf[a];
  uint8_t buff_d[b];
  LoRa.beginPacket();
  for (size_t k = 0; k < len_i; k++)
  {
    memcpy(buf,&msg_[k],a);
    for (uint8_t i = 0; i < a; i++)
    {
      LoRa.write(buf[i]);
    }
  }
  for (size_t k = 0; k < len_d; k++)
  {
    memcpy(buff_d,&msg__[k],b);
    for (uint8_t i = 0; i < b; i++)
    {
      LoRa.write(buff_d[i]);
    }
  }
  LoRa.endPacket(true);
  return true;
}

void Telemetry_m::SendTelemetryPack(bool start_, bool par_,bool par_speed, double height_, double speed_,
                                                         double Lon_,    double Lat_)
{
  packet++;
  uint8_t Len_=23;
  uint8_t msg_[Len_];

  uint8_t flag_=0;
  if(start_)flag_=1;
  if(par_)flag_+=2;
  if(par_speed)flag_+=4;

  int32_t Lon= (int32_t)(Lon_*1000000.0);
  int32_t Lat= (int32_t)(Lat_*1000000.0);

  msg_[0]='*';
  memcpy(msg_+1,&packet,4);
  memcpy(msg_+5,&height_,4);
  memcpy(msg_+9,&speed_,4);
  memcpy(msg_+13,&Lon,4);
  memcpy(msg_+17,&Lat,4);
  memcpy(msg_+21,&flag_,1);
  msg_[22]='#';

  LoRa.beginPacket();
  for (size_t i = 0; i < Len_; i++)
  {
    LoRa.write(msg_[i]);
  }
  
  LoRa.endPacket(true);
  
}




//////////////////////////////////////////////////////////////////Recive
bool Telemetry_m::AvailablePacket()
{
  if(LoRa.parsePacket())return true;
  else return false;
}

String Telemetry_m::ReciveS()
{
  String msg="";
  while (LoRa.available()) {
    msg+=(char)LoRa.read();
  }
  return msg;
}

bool Telemetry_m::ReciveIntDouble(int32_t *msg_,uint8_t len_i, double* msg__,uint8_t len_d)
{
  uint8_t a=sizeof(int32_t);
  uint8_t b=sizeof(double);

  uint8_t buf[a];
  uint8_t buff_d[b];
  if (LoRa.available() == (len_i + len_d))
  {
    for (uint8_t k = 0; k < len_i; k++)
    {
      for (uint8_t i = 0; i < a; i++)
      {
        buf[i] = LoRa.read();
      }
      memcpy(msg_ + a * k, buf, a);
    }
    for (uint8_t k = 0; k < len_d; k++)
    {
      for (uint8_t i = 0; i < b; i++)
      {
        buff_d[i] = LoRa.read();
      }
      memcpy(msg__ + b * k, buff_d, b);
    }
    return true;
  }
  else
  {
    while (LoRa.available()) {buf[0]=LoRa.read();}
    return false;
  }
}

String Telemetry_m::ParceTelemetryPack()
{
  uint8_t len_=23;
  String msg="";
  uint8_t msg_[len_];

  uint32_t Buff=0;
  double Buff_d=0.0;

  if(LoRa.available()==len_)
  {
    for (size_t i = 0; i < len_; i++)
    {
      msg_[i]=LoRa.read();
    }

    packet++;

    if(msg_[0]=='*' && msg_[22]=='#')
    {
      memcpy(&Buff,&msg_[1],4); //packet
      msg+="R:"+String(Buff);

      msg+="/"+String(packet); //My packet 

      memcpy(&Buff_d,&msg_[5],4); //height
      msg+=":"+String(Buff_d);

      memcpy(&Buff_d,&msg_[9],4); //speed
      msg+=":"+String(Buff_d);

      memcpy(&Buff,&msg_[13],4); //Lon
      Buff_d=Buff/1000000.0;
      msg+=":"+String(Buff_d,6);
      
      memcpy(&Buff,&msg_[17],4); //Lat
      Buff_d=Buff/1000000.0;
      msg+=":"+String(Buff_d,6);

      Buff=msg_[21]; //flag
      msg+=":"+String(Buff&0b00000001);
      msg+=":"+String((Buff&0b00000010)>>1);
      msg+=":"+String((Buff&0b00000100)>>2);

      msg+="\n";

      return msg;
    }
    return "";
  }
  else return ReciveS();
  
}