/**
    @brief Базовый класс для часов
*/
#ifndef CLOCK_H
#define CLOCK_H

#include "shape.h"

class Clock : public Shape {
  public:
    /**
        @brief Конструктор.  

        @param x Координата X левого верхнего угла.
        @param y Координата Y левого верхнего угла.
        @param w Ширина метки в пикселах.
        @param h Высота метки в пикселах.
        @param tft Ссылка на дисплей.
    */
    Clock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, TFT_eSPI* tft)
        : Shape(x, y, w, h, tft) {
    }
    /**
        @brief Установка времени.

        @param h Часы.
        @param m Минуты.
        @param s Секунды.
    */
    void setTime(uint8_t h, uint8_t m, uint8_t s) {
        hour = h;
        minute = m;
        second = s;
        draw();
    }
    /**
        @brief Установка даты.

        @param y Год.
        @param m Месяц.
        @param d День.
    */
    void setDate(uint16_t y, uint8_t m, uint8_t d) {
      year = y;
      month = m;
      day = d;
      draw();
    }

    String getTimeHH24MI() {
        char buf[5];
        sprintf(buf, "%02d:%02d", hour, minute);
        return String(buf);
    }

    String getTimeHH24MISS() {
        char buf[8];
        sprintf(buf, "%02d:%02d:%0d", hour, minute, second);
        return String(buf);
    }

  protected:
    uint8_t hour;    //!< Часы.
    uint8_t minute;  //!< Минуты.
    uint8_t second;  //!< Секунды.
    uint16_t year;   //!< Год.
    uint8_t month;   //!< Месяц.
    uint8_t day;     //!< День.
}; // Clock class

#endif