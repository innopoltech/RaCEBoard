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
//#define VIRTUAL_HEIGHT

/*
 * PARAMETERS
 */
#ifdef VIRTUAL_HEIGHT
   double        M_Height                = 600;
#endif
 
unsigned long openSolarBattery          = 10000;     //Время поджига нити
unsigned long offAfterLandTime          = 200000;   //Время выключения систем телеметрии и записи на карту памяти 

double        H_START                   = 10;       //Контрольная высота (m) для детекции старта
double        A_START                   = 4;        //Контрольное ускорения (G) для детекции старта
double        PH_IGNITE                 = 1.5;      //КОнтрольное напряжение на фоторезисторе (V) для открытия солнечных панелей

unsigned long T_ADC                     = 100;      //Периoд опроса АЦП
unsigned long T_GPS                     = 200;      //Периoд опроса GNSS
unsigned long T_IMU                     = 50;       //Периoд опроса IMU
unsigned long T_SD                      = 1000;     //Периoд обновления SD
unsigned long T_TELEMETRY               = 1000;     //Периoд отправки радиопакетов

unsigned long GPS_Baud                  = 9600;   //Скорость UART GPS (9600-c3

/*
 * DEFINE
 */

#define RemovePin A8
#define BntMainPin A10
#define HeaterOnePin 5
#define HeaterTwoPin 3

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

ADC_m Photoresistor_V(A0,1.0);
ADC_m Battery_V(A1,1.91);
ADC_m Solar_mA(A2,58.823);



/*
 * FLAG
 */
bool AllModuleInit          = true;
bool RadioInit              = false;
bool CardInit               = false;
bool BarInit                = false;

bool StartWait            = false;
bool StartFlight            = false;
bool Ignite                 = false;

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

unsigned long IgniteTime                    = 0;
double accel[4];    double gyro[3];     double mag[3];

void setup() {
  delay(300);
  /*
   * INIT I/O
   */
  pinMode(RemovePin,INPUT);      pinMode(BntMainPin,INPUT);
  pinMode(HeaterOnePin,OUTPUT);         pinMode(HeaterTwoPin,OUTPUT);
  digitalWrite(HeaterOnePin,HIGH);      digitalWrite(HeaterTwoPin,HIGH);
  /*
   * INIT MODULE
   */
   //for(int i=0;i<2;i++){
  //S_DEBUG                //Auto Update Data       //Telemetry          //SDCard    
    Serial.begin(115200);   TelemetryEn();  AUpd.enableTimer();             SDCardEn();   //Telemetry must be before the card 

  //GNSS                   //Barometer             //IMU                //ADC
    GNSSEn();                BarEn();                IMUEn();            ADCEn();
   //}
  #ifdef VIRTUAL_HEIGHT
    v_0=sqrt(2*9.81*M_Height); 
  #endif


  /*
   * CAMERA SETUP
   */


  if(AllModuleInit)BuzzerOnTime(1000); //All Done

  
  while (digitalRead(RemovePin)) //Ожидание удаления ремувки
  {    
    Bar.Upd();
  }
  StartWait=true;
  if(S_DEBUG)Serial.println("Готовность к полету номер 1 !");
  delay(100);
  if(CardInit){BoardCard.println("Готовность к полету номер 1 !");}
  if(RadioInit){Telemetry.SendS("Готовность к полету номер 1 !\n");} 
  
  
  }

void loop() {
  while(
    //TimeNow<offAfterLandTime || !StartFlight
    true
    ) //В полете
  {
    /*
     * DATA SECTION
     */
         
    if(TimeNow>T_ADC_Time)  //Обновление данных с АЦП
    {
      Msg_ADC="A:"+String(TimeNow)+":"+String(Photoresistor_V.Measure())+":"+String(Battery_V.Measure())+":"+String(Solar_mA.Measure());
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
      //Serial.println("UPD");
      BoardCard.upd();
      T_SD_Time=TimeNow+T_SD;
    }

    if(TimeNow>T_TELEMETRY_Time)  //Передача пакета телеметрии
    {
      Telemetry.SendTelemetryPack(StartFlight, StartWait,Ignite  ,Height_BAR ,Speed_BAR  ,Longitude_GPS ,Latitude_GPS);
      T_TELEMETRY_Time=TimeNow+T_TELEMETRY;
    }



    /*
     * PROGRAMM SECTION
     */

      
      if (!StartFlight && 
          Height_BAR>H_START &&
          accel[3]>A_START )  // Детекция старта : сглаженная высота больше х , суммарное ускорение больше у
      {
          StartFlight=true;
          StartTime=millis();
          T_ADC_Time  = 0;  T_GPS_Time        = 0;  T_IMU_Time    = 0;          
          T_SD_Time   = 0;  T_TELEMETRY_Time  = 0;  IgniteTime    = 0;
          BuzzerOnTime(250);
          if(S_DEBUG)Serial.println("Старт!");
          if(CardInit){BoardCard.println("Старт!");}
          /*ENABLE VIDEO CAP*/
      }

      if (StartFlight && 
          !Ignite &&
          Photoresistor_V.Measure()>PH_IGNITE )  // Открытие солнечных панелей
      {
        IgniteTime=TimeNow+openSolarBattery;
        Ignite=true;
        digitalWrite(HeaterOnePin,LOW); digitalWrite(HeaterTwoPin,LOW); 
      }


      
      if (TimeNow>IgniteTime) //Выключение поджигателя и обновление времени
      {
        digitalWrite(HeaterOnePin,HIGH); digitalWrite(HeaterTwoPin,HIGH); 
      }

      /*
      if (TimeNow>DCTime) //Осtановка сервы
      {
        DCMotorSetSpeed(0);
      }
      */


    
      
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
  
    //pinMode(Buzzer_Pin,OUTPUT); //Buzz En
  
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
