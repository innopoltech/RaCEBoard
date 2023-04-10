#ifndef  SDCustom_h
#define  SDCustom_h

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>


class SDCard_m{
    public:
        SDCard_m(){} //Конструктор
        bool begin(String msg_); //Инициализация карты, и запись первой строки
        void print(String msg_); //Запись строки в буфер, без символа переноса
        void println(String msg_); //Запись строки в буфер, с символом переноса
        void end(); //Завершение работы с картой памяти
        void upd(); //Сброс данных с буфера на карту
    
    private:
        File LogFile;
        bool FileOk=false;
};

/* Example
#include <SDCustom.h>
SDCard_m BoardCard;

BoardCard.begin("Добро пожаловать!")
BoardCard.print("Надпись без переноса ");
BoardCard.println("Надпись с переносом  ");
BoardCard.upd();
BoardCard.end();
delay(100);
*/

#endif
