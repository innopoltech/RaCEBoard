#include "ADC.h"


float ADC_m::Measure(){
   uint16_t Buff = analogRead(Pin);
   float Result = (Buff*5.0)/1024.0;
   Result *= Div;
   return Result;
}
