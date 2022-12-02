#ifndef Telemetry_h
#define Telemetry_h

#include <Arduino.h>
#include "LoRa_promini.h"

typedef enum {Channel_1,Channel_2,Channel_3,Channel_4,Channel_5,Channel_6} Channels;

class Telemetry_m
{
    public:
        Telemetry_m()//Конструктор
        {} 
        uint8_t on(); //Включить радиомодуль
        void off(); //Выключить радиомодуль
        void SetLowPower(); //Режим низкой мощности 
        void SetMaxPower(); //Режим максимальной мощности
        void SetChannel(Channels ch_); //Установить канал 1-6
        //Send
        void SendS(String msg_); //Отправка строки в блокирующем режиме
        void AsyncOn(); //Включение асинхронной передачи
        bool AsyncSendS(String msg_); //Отправка строки в не-блокирующем режиме
                                    // (false - модуль занят, true - модуль принял сообщение )
        void SendIntDouble(int32_t *msg_,uint8_t len_i, double* msg__,uint8_t len_d); //Отправка int (затем) double массивов в блокирующем режиме
        bool AsyncSendIntDouble(int32_t *msg_,uint8_t len_i, double* msg__,uint8_t len_d); //Отправка int (затем) double массивов в не-блокирующем режиме
                                    // (false - модуль занят, true - модуль принял сообщение )

        void SendTelemetryPack(bool start_, bool par_,bool par_speed, double height_, double speed_,
                                                         double Lon_,    double Lat_); //Отправляет пакет отслеживания на приемную станцию
        //Recive
        bool AvailablePacket(); //Проверка наличия целостных сообщений
        String ReciveS(); //Прием строки в блокирующем режиме
        bool ReciveIntDouble(int32_t *msg_,uint8_t len_i, double* msg__,uint8_t len_d); //Парсинг 
                        //сообщения на Int и Double массивы (размерности должны совпадать с отправкой)
                        // (false -размерности не совпадают(посылка отклоняется), true - размерности совпадают(посылка принимается) )
        String ParceTelemetryPack(); //Принимает пакет отслеживания
    private:
        uint8_t initialT;
};


/* Example
#include "src/Telemetry/Telemetry.h"

Telemetry_m Telemetry;
String msg;
double msg_d[10];
int32_t msg_i[10];

//
Telemetry.on();
for (uint8_t i = 0; i < 10; i++){ msg_d[i]=i*2.25;}
for (uint8_t i = 0; i < 10; i++){ msg_i[i]=i*3;}

Telemetry.SetLowPower();
Telemetry.SetMaxPower();

Telemetry.SendS(msg);
Telemetry.SendIntDouble(msg_i,msg_d);

Telemetry.AsyncOn();

Telemetry.AsyncSendS(msg);
Telemetry.AsyncSendIntDouble(msg_i,msg_d);

msg= Telemetry.ReciveS();
Telemetry.ReciveIntDouble(msg_i,msg_d);
//

Telemetry.off();
*/

#endif
