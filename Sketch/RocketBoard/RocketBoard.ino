/*
 * INCLUDE
 */
#include<Macros.h>
#include <AutoUpdate.h>
#include<Telemetry.h>
#include<GPS.h>
#include<SDCustom.h>
#include <ADC.h>
#include <TenDOF.h>

#define S_DEBUG true
#define VIRTUAL_HEIGHT

/*
 * PARAMETERS
 */
#ifdef VIRTUAL_HEIGHT
   double        M_Height                = 500;
#endif
unsigned long closeSafeSystemTime       = 3500;     //Время работы двигателя на закрытие
unsigned long openSafeSystemTime        = 7000;     //Время работы двигателя на открытие
unsigned long ApogeeTime                = 10500;    //Расчетной время время достижения апогея
unsigned long offAfterLandTime          = 200000;    //Время выключения систем телеметрии и записи на карту памяти 
unsigned long TimeWorkEngine            = 2500;     //Время работы двигателя
unsigned long VirtualSpeed              = 35;       //Виртуальная скорость спуска s

unsigned long T_ADC                     = 100;      //Периoд опроса АЦП
unsigned long T_GPS                     = 200;      //Периoд опроса GNSS
unsigned long T_IMU                     = 50;       //Периoд опроса IMU
unsigned long T_SD                      = 1000;     //Периoд обновления SD
unsigned long T_TELEMETRY               = 1000;      //Периoд отправки радиопакетов

unsigned long GPS_Baud                  = 9600;   //Скорость UART GPS

/*
 * DEFINE
 */

#define JumperPin A8
#define BntMainPin A10

#define OpenDir (-100)

#define TimeNow (millis()-StartTime)
/*
 * Calibration IMU
 */
double CalibAccel[4][3]={
          {0,0,0},  //ofsett
          {1,0,0},   //first line
          {0,1,0},   //second line
          {0,0,1}};  //third line
double CalibGyro[3]={0,0,0};  //ofsett
double CalibMag[4][3]={
          {0,0,0},  //ofsett
          {1,0,0},   //first line
          {0,1,0},   //second line
          {0,0,1}};  //third line


/*
 * CLASS
 */
AutoUpd_m AUpd;         Telemetry_m Telemetry;          SDCard_m BoardCard;
GPS_m GPS(GPS_Baud);    Barometer_m Bar;                IMU_m IMU;

ADC_m Battery_V(A1,1.91);



/*
 * FLAG
 */
bool AllModuleInit          = true;
bool RadioInit              = false;
bool CardInit               = false;
bool BarInit                = false;

bool StartFlight            = false;
bool StartSafeSystem        = false;
bool StartSafeSystem_speed  = false;

/*
 * PRIVATE PARAMETER
 */
uint8_t       BntMain_Step              = 0;
unsigned long StartTime                 = 0;

String        Msg_ADC                   = "";
unsigned long T_ADC_Time                = 0;

String        Msg_GPS                   = "";
unsigned long T_GPS_Time                = 0;
double        Longitude_GPS             = 0;
double        Latitude_GPS              = 0;

String        Msg_BAR                   = "";
double        Height_BAR                = 0;
double        Speed_BAR                 = 0;

String        Msg_IMU                   = "";
unsigned long T_IMU_Time                = 0;

unsigned long T_SD_Time                 = 0;

unsigned long T_TELEMETRY_Time          = 0;

#ifdef VIRTUAL_HEIGHT
  double        Height_VBAR             = 0.0;
  double        d_time                  = T_IMU/1000.0;
  double        v_0                     = 0.0;
#endif

unsigned long DCTime                    = 0;
double accel[4];    double gyro[3];     double mag[3];

void setup() {
  /*
   * INIT I/O
   */
  pinMode(JumperPin,INPUT);      pinMode(BntMainPin,INPUT);
  /*
   * INIT MODULE
   */
  //S_DEBUG                //Auto Update Data       //Telemetry          //SDCard    
    Serial.begin(115200);     AUpd.enableTimer();      TelemetryEn();      SDCardEn();   //Telemetry must be before the card 

  //GNSS                   //Barometer             //IMU                //ADC
    GNSSEn();              BarEn();                IMUEn();            ADCEn();

  if(AllModuleInit)BuzzerOnTime(1000); //All Done
  
  #ifdef VIRTUAL_HEIGHT
    v_0=sqrt(2*9.81*M_Height); 
  #endif


  /*
   * PREPARE MODE
   */
  while (digitalRead(JumperPin))
  {    
    if(digitalRead(BntMainPin)==HIGH)
    {
       if(!BntMain_Step) //Закрываем
       {
          BntMain_Step=1;
          DCMotorSetSpeed(-OpenDir);
          delay(closeSafeSystemTime);
          DCMotorSetSpeed(0);
       }
       else if(BntMain_Step)  //Открываем
       {
          BntMain_Step=0;
          DCMotorSetSpeed(OpenDir);
          delay(openSafeSystemTime);
          DCMotorSetSpeed(0);
       }
    }
    Battery_V.Measure();
    Bar.Upd();
  }
  StartFlight=true;
  StartTime=millis();
  BuzzerOnTime(250);
  if(S_DEBUG)Serial.println("Старт!");
  if(CardInit){BoardCard.println("Старт!");}
  if(RadioInit){Telemetry.SendS("Старт!\n");} 
  delay(1);
 
    
  }

void loop() {
  while(//TimeNow<offAfterLandTime
        true) //В полете
  {
    /*
     * DATA SECTION
     */
         
    if(TimeNow>T_ADC_Time)  //Обновление данных с АЦП
    {
      Msg_ADC="A:"+String(TimeNow)+":0:"+String(Battery_V.Measure())+":0";
      if(S_DEBUG)Serial.println(Msg_ADC);
      if(CardInit)BoardCard.println(Msg_ADC);
      T_ADC_Time=TimeNow+T_ADC;
    }

    if(TimeNow>T_GPS_Time)  //Обновление данных с GNSS
    {
      Msg_GPS="G:"+String(TimeNow)+":"+GPS.LonLatAlt();
      Longitude_GPS=GPS.Longitude();
      Latitude_GPS=GPS.Latitude();
      if(S_DEBUG)Serial.println(Msg_GPS);
      if(CardInit)BoardCard.println(Msg_GPS);
      T_GPS_Time=TimeNow+T_GPS;
    }


    #ifdef VIRTUAL_HEIGHT //Виртуальная отладка
      if(Serial.available()>=6)
      {
        Height_VBAR=ReadVBAR(); 
      }
    #endif

#ifdef VIRTUAL_HEIGHT
    if (!Bar.Upd(Height_VBAR)) //Обновление данных с BAR - эмуляция
#else
    if (!Bar.Upd()) //Обновление данных с BAR
#endif
    {
      Msg_BAR="B:"+String(TimeNow)+":"+String(Bar.GetTemp())+":"+String(Bar.GetHeight())+":"+String(Bar.GetHeightK())+":"+String(Bar.GetVertSpeed());
      if(S_DEBUG)Serial.println(Msg_BAR);
      if(CardInit)BoardCard.println(Msg_BAR);
      Height_BAR=Bar.GetHeightK();
      Speed_BAR=Bar.GetVertSpeed();
    }

    if(TimeNow>T_IMU_Time)  //Обновление данных с IMU
    {
      IMU.GetAccel(accel);    IMU.GetGyro(gyro);    IMU.GetMag(mag);
      Msg_IMU="I:"+String(TimeNow)+":"+
                   String(accel[0])+":"+String(accel[1])+":"+String(accel[2])+":"+String(accel[3])+":"+
                   String(gyro [0])+":"+String(gyro [1])+":"+String(gyro [2])+":"+
                   String(mag  [0])+":"+String(mag  [1])+":"+String(mag  [2]);
      if(S_DEBUG)Serial.println(Msg_IMU);
      if(CardInit)BoardCard.println(Msg_IMU);
      T_IMU_Time=TimeNow+T_IMU;

#ifdef VIRTUAL_HEIGHT
      Height_VBAR+=v_0*d_time;
      v_0-=9.81*d_time;
#endif
    }

    if(TimeNow>T_SD_Time)  //Обновление данных на SDCard
    {
      BoardCard.upd();
      T_SD_Time=TimeNow+T_SD;
    }

    if(TimeNow>T_TELEMETRY_Time)  //Передача пакета телеметрии
    {
      Telemetry.SendTelemetryPack(StartFlight, StartSafeSystem  ,StartSafeSystem_speed  ,Height_BAR ,Speed_BAR  ,Longitude_GPS ,Latitude_GPS);
      T_TELEMETRY_Time=TimeNow+T_TELEMETRY;
    }



    /*
     * PROGRAMM SECTION
     */

      
      if (!StartSafeSystem && TimeNow > ApogeeTime)  // открытие по таймеру
      {
        String msg_="Открытие парашюта по таймеру!";
        DCTime=TimeNow+openSafeSystemTime;
        DCMotorSetSpeed(OpenDir);
        StartSafeSystem = true; 
        if(S_DEBUG)Serial.println(msg_);
        if(CardInit)BoardCard.println(msg_);
      }
      if (!StartSafeSystem_speed && (TimeNow > TimeWorkEngine) && (Speed_BAR<VirtualSpeed)  ) //открытие по скорости
      {
        String msg_="Открытие парашюта по скорости!";
        DCTime=TimeNow+openSafeSystemTime;
        DCMotorSetSpeed(OpenDir);
        StartSafeSystem_speed = true; 
        if(S_DEBUG)Serial.println(msg_);
        if(CardInit)BoardCard.println(msg_);
      }
      
      if (TimeNow>DCTime) //Осtановка сервы
      {
        DCMotorSetSpeed(0);
      }


    
      
    }
                                //После приземления
    String msg_="End Log!";
    if(S_DEBUG)Serial.println(msg_);
    if(CardInit){BoardCard.println(msg_);BoardCard.end();}

    while(1){}                  //Конец полета
}






/*
 * Enable Module
 */

void TelemetryEn()
{
    String msg_ok="Радиомодуль запущен!";
    String msg_err="Ошибка инициализации радиомодуля!";
  
    pinMode(Buzzer_Pin,OUTPUT); //Buzz En
  
    if(Telemetry.on()==1)
    {
      RadioInit=true;
      Telemetry.SetMaxPower();
      Telemetry.SetChannel(Channel_1);
      if(S_DEBUG)Serial.println(msg_ok);
    }
    else
    {
      if(S_DEBUG)Serial.println(msg_err);
      AllModuleInit=false;
      BuzzerOn;
    }
}

void SDCardEn()
{
    String msg_ok="Модуль карты памяти запущен!";
    String msg_err="Ошибка инициализации карты памяти!";
  
    if(BoardCard.begin("Start Log"))
    {
      CardInit=true;
      if(S_DEBUG)Serial.println(msg_ok);
      if(RadioInit)Telemetry.SendS(msg_ok+"\n"); 
      RadioInit?BoardCard.println("Радиомодуль запущен!"):BoardCard.println("Ошибка инициализации радиомодуля!");
    }
    else
    {
      if(S_DEBUG)Serial.println(msg_err);
      if(RadioInit)Telemetry.SendS(msg_err+"\n"); 
      AllModuleInit=false;
      BuzzerOn;
    }
}

void GNSSEn()
{   
    String msg_ok="Навигационный модуль запущен!";
    String msg_err="Ошибка инициализации навигационного модуля!";
  
    AUpd.GPSEn(30);
    GPS.StartTrack();
    delay(1500);
    if( GPS.IsUpdated() || GPS.existRec )
    {
      if(S_DEBUG)Serial.println(msg_ok);
      if(CardInit)BoardCard.println(msg_ok); 
      if(RadioInit)Telemetry.SendS(msg_ok+"\n"); 
    }
    else
    {
      if(S_DEBUG)Serial.println(msg_err);
      if(CardInit)BoardCard.println(msg_err); 
      if(RadioInit)Telemetry.SendS(msg_err+"\n"); 
      AllModuleInit=false;
      BuzzerOn;
    }
}

void ADCEn()
{
    String msg_="Напряжение на аккумуляторе В: ";
    
    float V=Battery_V.Measure();
    if(S_DEBUG){Serial.print(msg_);Serial.println(V);}
    if(CardInit){BoardCard.print(msg_);BoardCard.println(String(V)); }
    if(RadioInit){Telemetry.SendS(msg_);Telemetry.SendS(String(V));Telemetry.SendS("\n"); } 
}


void BarEn()
{
    String msg_ok="Барометрический модуль запущен!";
    String msg_err="Ошибка инициализации барометрического модуля!";
  
    if(Bar.begin())
    {
      BarInit=true;
      delay(200);
      Bar.SetZeroHeight();
      AUpd.BarEn(10);
      if(S_DEBUG)Serial.println(msg_ok);
      if(CardInit)BoardCard.println(msg_ok); 
      if(RadioInit)Telemetry.SendS(msg_ok+"\n"); 
    }
    else
    {
      if(S_DEBUG)Serial.println(msg_err);
      if(CardInit)BoardCard.println(msg_err); 
      if(RadioInit)Telemetry.SendS(msg_err+"\n"); 
      AllModuleInit=false;
      BuzzerOn;
    }
}

void IMUEn()
{
    String msg_ok="Инерциальный модуль запущен!";
    String msg_err="Ошибка инициализации инерциального модуля!";
  
    if(IMU.begin())
    {
      IMU.SetCalibAccel(CalibAccel[0],CalibAccel[1],CalibAccel[2],CalibAccel[3]);
      IMU.SetCalibGyro(CalibGyro);
      IMU.SetCalibMag(CalibMag[0],CalibMag[1],CalibMag[2],CalibMag[3]);
      if(S_DEBUG)Serial.println(msg_ok);
      if(CardInit)BoardCard.println(msg_ok); 
      if(RadioInit)Telemetry.SendS(msg_ok+"\n"); 
    }
    else
    {
      if(S_DEBUG)Serial.println(msg_err);
      if(CardInit)BoardCard.println(msg_err); 
      if(RadioInit)Telemetry.SendS(msg_err+"\n"); 
      AllModuleInit=false;
      BuzzerOn;
    }
}


#ifdef VIRTUAL_HEIGHT
  double ReadVBAR()
  {
    uint8_t buff[6];
    int32_t h_=0;
    double h=0;
    Serial.readBytes(buff,6);
    if(buff[0]=='*' && buff[5]=='#')
    {
      memcpy(&h_,&buff[1],4);
      h=h_/100.0;
    }

    while(Serial.available())Serial.read();
    return h;
  }
#endif


void pass(){;}
