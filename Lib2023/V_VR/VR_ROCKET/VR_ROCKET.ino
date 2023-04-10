#include "VirtualDevice.h"


GPIO_m j;
IMU_m i;
Barometer_m b;

double bf[3];

void setup()
{
  Serial1.begin(115200);
  i.begin();
  b.begin();
}



void loop()
{
  float h = b.GetHeight();
  String msg = "";
   msg +=String(millis());
    msg += String(j.digitalRead_jump())+"\t";
    msg += String(h)+"\t";

    i.GetAccel(bf);
    msg += String(bf[0])+"\t"+String(bf[1])+"\t"+String(bf[2])+"\t";
    i.GetGyro(bf);
    msg += String(bf[0])+"\t"+String(bf[1])+"\t"+String(bf[2])+"\t";
    
    
    if(h>50)
      j.digitalWrite_prsht();

    //b.SetZeroHeight();
  
  Serial1.println(msg);

  delay(100);
}
