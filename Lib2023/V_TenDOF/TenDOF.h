#ifndef  TenDOF_h
#define  TenDOF_h

#include <Arduino.h>
#include "BMP280.h"
#include "MPU9250.h"

//Блок BMP280
class Barometer_m{
    public:
        Barometer_m(){} //Конструктор

        bool begin();               //Инициализровать барометр
        double GetTemp();           //Получить температуру, в градусах цельсия
        double GetHeight();         //Получить высоту, в метрах
        void SetZeroHeight();       //Установить нулевую высоту
    private:
        float ZeroHeight=0;
};



//Блок MPU9250
class IMU_m
{
    public:
        IMU_m(){} //Конструктор

        bool begin();                   //Инициализация датчика
        bool GetAccel(double * bf_);    //Получить вектор данных с акселерометра [x,y,z,Sqrt] 
        bool GetGyro(double * bf_);     //Получить вектор данных с гироскопа [x,y,z]
        //bool GetMag(double * bf_);      //Получить вектор данных с магнитометра [x,y,z]

        void WhileCalib();  //Запуск калибровки, выполняется бесконечно
        void SetCalibAccel(double * ac1,double * ac2,double * ac3,double * ac4); //Установить калибровочные коэффициенты (см документацию)
        void SetCalibGyro(double * clb_); 
        //void SetCalibMag(double * ma1,double * ma2,double * ma3,double * ma4);
    private:
        double CalibAccel[4][3]={
                {0,0,0},  //ofsett
                {1,0,0},   //first line
                {0,1,0},   //second line
                {0,0,1}};  //third line
        double CalibGyro[3]={0,0,0};  //ofsett
        // double CalibMag[4][3]={
        //         {0,0,0},  //ofsett
        //         {1,0,0},   //first line
        //         {0,1,0},   //second line
        //         {0,0,1}};  //third line

};






/*Example Barom

#include <TenDOF.h>
Barometer_m Bar;

Bar.begin();
Bar.SetZeroHeight();

Serial.print(Bar.GetTemp()); Serial.print(" : ");
Serial.print(Bar.GetHeight()); Serial.print(" : ");
*/

/*Example IMU

#include<TenDOF.h>

IMU_m IMU;
double accel[4];
double gyro[3];

double CalibAccel[4][3]={
          {0,0,0},  //ofsett
          {1,0,0},   //first line
          {0,1,0},   //second line
          {0,0,1}};  //third line
double CalibGyro[3]={0,0,0};  //ofsett


IMU.begin();
IMU.SetCalibAccel(CalibAccel[0],CalibAccel[1],CalibAccel[2],CalibAccel[3]);
IMU.SetCalibGyro(CalibGyro);

IMU.GetAccel(accel);
IMU.GetGyro(gyro);

Serial.print(accel[0]); Serial.print(":");
Serial.print(accel[1]); Serial.print(":");
Serial.print(accel[2]); Serial.print(": ");

Serial.print(gyro[0]); Serial.print(":");
Serial.print(gyro[1]); Serial.print(":");
Serial.println(gyro[2]);

*/

#endif