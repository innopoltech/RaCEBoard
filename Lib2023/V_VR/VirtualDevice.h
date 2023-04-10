#ifndef _VIRTUAL_DEVICE__
#define _VIRTUAL_DEVICE__

#include <Arduino.h> 



class Barometer_m{
  private:
    bool init = false;
    double ZeroHeight = 0;
    double Height = 0;
  public:
    Barometer_m(){};
    bool begin();   //Инициализровать барометр
    double GetHeight(); //Получить высоту, в метрах
    void SetZeroHeight(); //Установить нулевую высоту
    double GetTemp(){return 20.0}; //Получить температуру, в градусах цельсия
};

class IMU_m{
  private:
    bool init = false;
  public:
    IMU_m(){};
    bool begin(); //Инициализация датчика
    bool GetAccel(double * bf_); //Получить вектор данных с акселерометра [x,y,z,Sqrt] 
    bool GetGyro(double * bf_); //Получить вектор данных с гироскопа [x,y,z]

    void WhileCalib(){while(true){;}};  //Запуск калибровки, выполняется бесконечно
    void SetCalibAccel(double * ac1,double * ac2,double * ac3,double * ac4){;}; //Установить калибровочные коэффициенты (см документацию)
    void SetCalibGyro(double * clb_){;}; 

};

class GPIO_m{
  private:
  public:
     Jumper_m(){};
     uint8_t digitalRead_jump(); //Получить значения джампера
     void digitalWrite_prsht();  //Установить значения выброса парашюта
};









enum Codes{
  JUMPER_digitalRead_jump = 0,
  JUMPER_digitalWrite_prsht,
  IMU_GetAccel,
  IMU_GetGyro,
  BARO_GetHeight,
};

class VirtualDev_{
  private:
    bool init = false;
    uint8_t msg_tx[32];
    uint8_t msg_rx[32];
  
    void SerialClear();
    bool SerialWait(uint16_t x);
      
    bool ReciveCMD();
    uint8_t CRC8();
  public:
    VirtualDev_(){};

    bool FlagJumper = false;
    bool FlagAccel = false;
    bool FlagGyro = false;
    bool FlagBar = false;

    bool SendCMD(Codes cmd_);
    void ExtractData(uint8_t* ptr,uint8_t offset ,uint8_t len);

    
};


#endif
