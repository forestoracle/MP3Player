/** 
    @brief класс для визуальных компонентов. 
*/

#include <stdint.h>
#ifndef SHAPE_H
#define SHAPE_H

#include <TFT_eSPI.h>

class Shape {
  public:
    /**
        @brief Конструктор.  

        @param x Координата X левого верхнего угла.
        @param y Координата Y левого верхнего угла.
        @param w Ширина в пикселах.
        @param h Высота в пикселах.
        @param tft Ссылка на дисплей.
    */
    Shape(uint16_t x, uint16_t y, uint16_t w, uint16_t h, TFT_eSPI *tft)
        : x(x),       //x(constrain(x, 0, tft.width() - 1)),
          y(y),       //y(constrain(y, 0, tft.height() - 1)),
          width(w),   //width(constrain(w, 1, constrain(x, 0, tft.width() - 1) - tft.width())),
          height(h),  //height(constrain(h, 1, constrain(y, 0, tft.height() - 1) - tft.height())),
          display(tft) {
    }
    /**
        @brief Прорисовка элемента.
   */
    virtual void draw(void) = 0;
    /**
        @brief Получение ширины.

        @return Ширина элемента в пикселах.
    */
    uint16_t getWidth(void) {
        return width;
    }
    /**
        @brief Получение высоты.

        @return Высота элемента в пикселах.
    */
    uint16_t getHeight(void) {
        return height;
    }
    /**
        @brief Установка ширины.

        @param h Ширина в пикселах.
        @param redraw Переписовывать ли элемент.
    */
    void setWidth(uint16_t w, bool redraw = true) {
        width = w;
        if (redraw) draw();
    }
    /**
        @brief Установка высоты.

        @param h Высота в пикселах.
        @param redraw Переписовывать ли элемент.
    */
    void setHeight(uint16_t h, bool redraw = true) {
        height = h;
        if (redraw) draw();
    }
    /**
        @brief Установка цвета текста и фона

        @param foreground Цвет переднего плана, например текст.
        @param background Цвет заднего плана, например фон.
        @param redraw Перерисовывать ли элемент полностью. 
    */
    void setTextColor(uint32_t foregroud, uint32_t background, bool redraw = true) {
        foregroundColor = foregroud;
        backgroundColor = background;
        if (redraw) draw();
    }
  protected:
    TFT_eSPI *display;         //!< дисплей
    uint16_t x;                //!< координата X левого верхнего угла
    uint16_t y;                //!< координата Y левого верхнего угла
    uint16_t width;            //!< ширина
    uint16_t height;           //!< высота
    uint32_t foregroundColor;  //!< цвет шрифта
    uint32_t backgroundColor;  //!< цвет фона
};
#endif  // SHAPE_H
