/**
    @brief  Полоса прогресса 
*/
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <TFT_eSPI.h>
#include "shape.h"

/**
    @brief  Полоса прогресса 
*/
class ProgressBar : public Shape {
  public:
    ProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t min, uint16_t max, TFT_eSPI *tft)
        : Shape(x, y, w, h, tft), min(min), max(max) {
        foregroundColor = TFT_WHITE;
        backgroundColor = TFT_BLACK;
        valuePx = x;
    }
    /*
        Установка минимального значения
    */
    void setMin(uint16_t min) {
        ProgressBar::min = min;
    }
    /*
        Установка максимального значения
    */
    void setMax(uint16_t max) {
        ProgressBar::max = max;
    }
    /*
        Установка цвета столбика
    */
    void setForegroundColor(uint32_t color) {
        uint16_t save_value = value;
        foregroundColor = color;
        setValue(min);
        drawBackgroud();
        setValue(save_value);
    }
    /*
        Установка цвета фона
    */
    void setBackgroundColor(uint32_t color) {
        uint16_t save_value = value;
        backgroundColor = color;
        setValue(min);
        drawBackgroud();
        setValue(save_value);
    }
    /*
        Прорисовка полосы прогресса в соответствие со значением new_value
    */
    void setValue(uint16_t new_value) {
        // Текущее значение долнжно попадать в диапазон от min до max,
        // и здесь оно принудительно туда укладывается
        if (new_value == value) return;
        value = constrain(new_value, min, max);
        calcValuePx();
        draw();
    }
    /*
        Прорисовка полосы прогресса в соответствие с установленными значениями

        Для увеличения скорости прорисовки применяем слудующий приём
        Если новое значение valuePx > oldValuePx
        +--------------------------------+
        |==============|------------|    |  Дорисовывается прямоугольник между oldValuePx и valuePx цветом полосы прогресса
        +--------------|------------|-----
                    oldValuePx    valuePx  

        Если новое значение valuePx < oldValuePx
        +--------------------------------+
        |==============|------------|    |  Закрашивается прямоугольник между valuePx и oldValuePx цветом полосы фона
        +--------------|------------|-----
                    valuePx    oldvaluePx  

    */
    void draw(void) override {
        if (valuePx == oldValuePx) return;
        if (valuePx > oldValuePx) {
            // Дорисовываем прямоугольник
            display->fillRect(oldValuePx, y, valuePx - oldValuePx, height, foregroundColor);
        } else {
            // Стираем лишнее
            display->fillRect(valuePx, y, oldValuePx - valuePx, height, backgroundColor);
        }
    }
    /*
        Прорисовка фона. 
    */
    void drawBackgroud() {
        display->fillRect(x, y, width, height, backgroundColor);
    }
  protected:
    uint16_t min;              // минимальное значение прогресса
    uint16_t max;              // максимальное значение прогресса
    uint16_t value;            // текущее значение прогресса
    uint16_t valuePx;          // текущее значение, спроецированное на экран в пикселях
    uint16_t oldValuePx;       // старое значение, спроецированное на экран в пикселях
    /*
      Вычисление отступа от x до правого края полосу прогресса.
      В пикселах!!!
    */
    void calcValuePx(void) {
        oldValuePx = valuePx;
        valuePx = (width / (max - min) * (value - min)) + x;
    }
};      // ProgressBar
#endif  // PROGRESSBAR_H