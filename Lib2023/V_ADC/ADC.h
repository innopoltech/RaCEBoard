
#ifndef ADC_h
#define ADC_h

#include <Arduino.h>

class ADC_m
{
  public:
    ADC_m(uint8_t Pin_, float Div_){//Конструктор, задаем пин, и множитель измерения
      Pin = Pin_;
      Div = Div_;
      pinMode(Pin, INPUT);
    }
    float Measure(); //Функция возвращает напряжение в вольтах

  private:
    uint8_t Pin;
    float Div;
};

/* Example
#include <ADC.h>

//ADC MODULE
ADC_m PIN_A0_V(A0,1.0);   
ADC_m Battery_V(A1,1.91);

//ADC MODULE
float a = PIN_A0_V.Measure();
float b = Battery_V.Measure();
*/

#endif
