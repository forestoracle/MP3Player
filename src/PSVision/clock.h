/**
    @brief Базовый класс для часов
*/
#ifndef CLOCK_H
#define CLOCK_H

#include "shape.h"

String weekDays_RU[7] = {"Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"};
String weekDaysShort_RU[7] = {"Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"};

class Clock : public Shape {
  public:
    /**
        @brief Конструктор.  

        @param x Координата X левого верхнего угла.
        @param y Координата Y левого верхнего угла.
        @param w Ширина в пикселах.
        @param h Высота в пикселах.
        @param tft Ссылка на дисплей.
    */
    Clock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, TFT_eSPI* tft)
        : Shape(x, y, w, h, tft) {
    }
    /**
        @brief Установка времени.

        @param h Часы (0..24).
        @param m Минуты (0..59).
        @param s Секунды (0..59).
    */
    void setTime(uint8_t h, uint8_t m, uint8_t s) {
        hour = constrain(h, 0, 24);
        minute = constrain(m, 0, 59);
        second = constrain(s, 0, 59);
        draw();
    }
    /**
        @brief Установка даты.

        @param y Год (0..3000).
        @param m Месяц (1..12).
        @param d День (1..31).
    */
    void setDate(uint16_t y, uint8_t m, uint8_t d) {
        year = constrain(y, 0, 3000);
        month = constrain(m, 1, 12);
        day = constrain(d, 1, 31);
        draw();
    }
    /** 
        @brief Возвращает время в формате HH24:MI 
    */
    String getTimeHH24MI() {
        char buf[5];
        sprintf(buf, "%02d:%02d", hour, minute);
        return String(buf);
    }

    /** 
        @brief Возвращает время в формате HH24:MI:SS 
    */
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
};  // Clock class

#endif