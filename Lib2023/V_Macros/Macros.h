#ifndef Macros_h
#define Macros_h

#include <Arduino.h>
#include <Servo.h>


//Buzzer
#define Buzzer_Pin 4              //Пин к которому подключен бузер
#define Buzzer_Frequency 2000     //Частота бузера в Гц

#define BuzzerOn() tone(Buzzer_Pin, Buzzer_Frequency)              //Активировать бузер
#define BuzzerOnTime(x)  tone(Buzzer_Pin, Buzzer_Frequency, (x) )  //Активировать бузер на некоторое время в мс
#define BuzzerOff() noTone(Buzzer_Pin)                             //Деактивировать бузер

//Servos
Servo ServoOne; Servo ServoTwo; Servo ServoThree; Servo ServoFour; 
#define ServoOneEn()    ServoOne.attach(11)     // Пин 1-го сервопривода на плате питания
#define ServoTwoEn()    ServoTwo.attach(12)     // Пин 2-го сервопривода на плате питания
#define ServoThreeEn()  ServoThree.attach(46)   // Пин 1-го сервопривода на плате инерциального датчика
#define ServoFourEn()   ServoFour.attach(45)    // Пин 2-го сервопривода на плате инерциального датчика

#define ServoOneSet(x)      ServoOne.write(x)     //Установить углы на сервоприводах, от 0 до 180
#define ServoTwoSet(x)      ServoTwo.write(x)     //Установить углы на сервоприводах, от 0 до 180
#define ServoThreeSet(x)    ServoThree.write(x)   //Установить углы на сервоприводах, от 0 до 180
#define ServoFourSet(x)     ServoFour.write(x)    //Установить углы на сервоприводах, от 0 до 180


//DC Motor
#define DCMotorPin1 6 //Пин 1-го входа мотора
#define DCMotorPin2 7 //Пин 2-го входа мотора

//#define DCMotor pinMode(14,INPUT_PULLUP) //Необязательно, настраивает пин слежения за перегрузкой по току
void  DCMotorZState()             //Отпускаем мотор в свободное вращение (Z-state)
{
  digitalWrite(DCMotorPin2,LOW);
  digitalWrite(DCMotorPin1,LOW);
}
void  DCMotorStop()               //Торможение мотором (блокировка ротора)
{
  digitalWrite(DCMotorPin2,HIGH);
  digitalWrite(DCMotorPin1,HIGH);
}
void  DCMotorSetSpeed(int y)      //Управление вращением мотора от -100% до 100%
{ 
  DDRG |= 1;
  PORTG |= 1; //Engine enable

  int x=constrain(y,-100,100);
  x>=0?digitalWrite(DCMotorPin2,HIGH):digitalWrite(DCMotorPin1,HIGH);
  x>=0?analogWrite(DCMotorPin1,255-(uint8_t)(x*2.55)):
       analogWrite(DCMotorPin2,255-(uint8_t)(-x*2.55));   //Управление вращением мотора , скорость в % от -100 до 100
}


/*DCMotor example 
  pinMode(DCMotorPin1,OUTPUT);
  pinMode(DCMotorPin2,OUTPUT);

  DCMotorSetSpeed(50);
*/
#endif
