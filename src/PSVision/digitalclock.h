/*

    День недели можно показывать как названием, так и диаграммой. Примерно такой:
    ====----- или такой ---=--- (можно реализовать как ProgressBar)
    

*/
#ifndef _DIGITALCLOCK_H
#define _DIGITALCLOCK_H

#include <stdint.h>
#include <TFT_eSPI.h>
#include "clock.h"


#define DIGITALCLOCK_FONT 7    //!< Это специальный шрифт, который выводит символы подобно семисегментному индикатору.
#define DIGITALCLOCK_ALPHA 20  // Используется как alpha при прорисовке "незажённых" ячеек
/**
    @brief Цифровые часы c возможностью показа времени, даты и дня недели.

    Используется шрифт 7. Это специальный шрифт (входит в библиотеку TFT_eSPI), 
    который выводит символы подобно семисегментному индикатору.
    Имеет очень ограниченный набор символов (цифры, :, ,).

    Размер символов 48 пикселов в высоту. Занимает во FLASH памяти примерно 2438 байт.
    Для использования шрифта нужно убедиться, что активна директива 
    ```cpp 
    #define LOAD_FONT7
    ```

*/
class DigitalClock : public Clock {
  public:
    DigitalClock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t c, TFT_eSPI* tft)
        : Clock(x, y, w, h, tft) {
        foregroundColor = c;
        backgroundColor = TFT_BLUE;
    }
    /**
        @brief Прорисовка часов
    */
    void draw(void) override {
        display->setViewport(x, y, width, height, true);
        // Рисуем пустые, "незажённые" ячейки индикатора
        // Здесь мы устанавдиваем текст фона, чтобы стереть текст, выведенный ранее
        // 0x39C4
        // При фоне TFT_BLACK alpha должна быть больше, при светлых - меньше. Надо как-то подбирать
        display->setTextColor(display->alphaBlend(DIGITALCLOCK_ALPHA, foregroundColor, backgroundColor), backgroundColor);
        display->drawString("88:88", 0, 0, DIGITALCLOCK_FONT);
        // Здесь мы не устанавливаем цвет фона, чтобы оставить прорисованные ранее "незажённые" ячейки
        display->setTextColor(foregroundColor);
        display->drawString(getTimeHH24MI(), 0, 0, DIGITALCLOCK_FONT);
        display->resetViewport();
    }
    /**
        @brief Показывать дату
    */
    void setShowDate(bool d) {
        showDate = d;
    }
    /**
        @brief Показывать день недели
    */
    void setShowWeekDay(bool d) {
        showWeekDay = d;
    }
  protected:
    bool showDate;     //!< Показывать дату
    bool showWeekDay;  //!< Показывать день недели
};
#endif