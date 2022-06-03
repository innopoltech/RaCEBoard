#include<Telemetry.h>
#include<SDCustom.h>
#include <AutoUpdate.h>

SDCard_m BoardCard;
AutoUpd_m AUpd; 
Telemetry_m Telemetry;


unsigned long T_SD                      = 1000;     //Периoд обновления SD
unsigned long T_SD_Time                 = 0;


String msg;


void setup() {

  pinMode(9,OUTPUT);
  digitalWrite(9,HIGH);
  
  Serial.begin(9600);
  Telemetry.on()==1?
    Serial.println("Radio ok!"):
    Serial.println("Error Radio!");
  Telemetry.SetMaxPower();
  Telemetry.SetChannel(Channel_5);


    if(BoardCard.begin("Start Log"))
    {
      Serial.println("Card_Ready");
    }
    else
    {
      Serial.println("Card_Error");
    }
}

//TEST




void loop() {

  
  if(Telemetry.AvailablePacket())
  {
    //Serial.print("Ch- ");Serial.println(Cha);
    msg=Telemetry.ParceTelemetryPack();
    Serial.print(msg);
    BoardCard.print(msg);
    //Serial.print("  ");Serial.print(LoRa.packetRssi());Serial.print("  ");Serial.println(LoRa.packetSnr());

  }

      if(millis()>T_SD_Time)  //Обновление данных на SDCard
    {
      BoardCard.upd();
      T_SD_Time=millis()+T_SD;
    }

    
}
