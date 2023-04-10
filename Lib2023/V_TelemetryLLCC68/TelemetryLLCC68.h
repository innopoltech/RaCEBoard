#ifndef Telemetry_h
#define Telemetry_h

#include <Arduino.h>


typedef enum {Channel_1,Channel_2,Channel_3,Channel_4,Channel_5,Channel_6} Channels;

class TelemetryLLCC68_m
{
    public:
        TelemetryLLCC68_m(){;} //Конструктор
        uint8_t on();                   //Включить радиомодуль
        void off();                     //Выключить радиомодуль
        void SetLowPower();             //Режим низкой мощности 
        void SetMaxPower();             //Режим максимальной мощности
        void SetChannel(Channels ch_);  //Установить канал 1-6
        //Send
        void SendS(String msg_);        //Отправка строки в блокирующем режиме
        void SendTelemetryPack(bool flag_0, bool flag_1,bool flag_2, double height_, double speed_,
                                                         double Lon_,    double Lat_); //Отправляет пакет отслеживания на приемную станцию
        //Recive
        bool AvailablePacket();         //Проверка наличия целостных сообщений
        String ReciveS();               //Прием строки в блокирующем режиме
        String ParceTelemetryPack();    //Принимает пакет отслеживания
    private:
        void init(long freq);
        int8_t powerTx = 10;
        uint8_t initialT=0;
};


/* Example
#include<TelemetryLLCC68.h>

Telemetry_m Telemetry;

Telemetry.on();
Telemetry.SetMaxPower();
Telemetry.SetChannel(Channel_3);

Telemetry.SendS("Hello!\n);
*/

#endif
