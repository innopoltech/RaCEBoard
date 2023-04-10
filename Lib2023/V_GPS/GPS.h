
#ifndef GPS_h
#define GPS_h

#include <Arduino.h>

#define GPS_UART Serial2

class GPS_m
{
    public:
        GPS_m(uint32_t Speed_)
        {
            Speed_s = Speed_; //Конструктор, задаем скорость обмена данными
        }

        void StartTrack();  //Начать работу
        void Work();        //Вызывать в loop как можно чаще, для обработки данных
        void StopTrack();   //Остановить работу
        bool IsUpdated();   //Актуальность данных (true-данные новые и не считывались)
        double Longitude(); //Долгота, в градусах
        double Latitude();  //Широта, в градусах
        double Speed();     //Горизонтальная скорость, в метрах в секунду
        double Altitude();  //Высота в метрах в секунду
        String LonLatAlt(); //Строка с координатами и высотой

        bool existRec =false;

    private:
        uint32_t Speed_s;
        bool existRec_=false;

};

/* Example
#include <GPS.h>

GPS_m GPS(9600);

GPS.StartTrack();

GPS.Work();
Serial.print("Longitude - ");Serial.println(GPS.Longitude(),6);
Serial.print("Latitude - ");Serial.println(GPS.Latitude(),6);
Serial.print("Speed - ");Serial.println(GPS.Speed());
Serial.print("Altitude - ");Serial.println(GPS.Altitude());
Serial.print("LatLonAlt - ");Serial.println(GPS.LonLatAlt());

*/


#endif
