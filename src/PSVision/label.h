/*
    Простая текстовая метка
*/

#ifndef LABEL_H
#define LABEL_H

#include "hardware/sync.h"
#include "hardware/address_mapped.h"
#include "fonts/verdanab12.h"  // надо переделать, чтобы не зависеть от конкретной папки
#include <stdint.h>

#include <TFT_eSPI.h>
#include "shape.h"
/**
    @brief Текстовая метка

    Простая текстовая метка с возможностью вывода иконки слева от текста.
*/
enum Align {
    alLeft,
    alRight,
    alCenter
};

class Label : public Shape {
  public:
    /**
        @brief Конструктор метки

        @param x Координата X левого верхнего угла
        @param y Координата Y левого верхнего угла
        @param w Ширина метки в пикселах
        @param h Высота метки в пикселах
        @param string Текст метки
        @param tft Ссылка на дисплей
    */
    Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String& string, TFT_eSPI* tft)
        : Shape(x, y, w, h, tft) {
        foregroundColor = TFT_WHITE;
        backgroundColor = TFT_BLACK;
        indentH = 1;
        indentV = 1;

        text = string;
        bitmap = nullptr;
    }

    Label(uint16_t x, uint16_t y, TFT_eSPI* tft)
        : Shape(x, y, 0, 0, tft) {
        foregroundColor = TFT_WHITE;
        backgroundColor = TFT_BLACK;
    }
    /**
        @brief Прорисовка текстовой метки.

        Цвет текста определяется foregroundColor, цвет фона - backgroundColor.
        
        Если весь текст не влезает в ширину метки, то он обрезается.
    */
    void draw(void) override {
        display->setViewport(x, y, width, height, true);          // Окно элемента
        display->setTextPadding(width);                           // Строка будет заполнена цветом фона на ширину width. .
        display->setTextColor(foregroundColor, backgroundColor);  // Установка цвета букв и фона
        if (bitmap != nullptr) {
            // иконка
            display->drawXBitmap(1, 0, bitmap, bitmapWidth, bitmapHeight, bitmapColor);
            display->drawString(text, indentH + bitmapWidth, indentV);
            // прорисовка строки внутри окна со сдвигом на ширину иконки
        } else {
            // прорисовка строки внутри окна
            uint32_t iH = indentH;
            switch (align) {
                case alCenter:
                    iH = width / 2;
                    display->setTextDatum(TC_DATUM);
                    break;
                case alRight:
                    iH = width - display->textWidth(text);
                    break;
                default:
                    break;
            }
            display->drawString(text, iH, indentV);
            display->setTextDatum(TL_DATUM);
        }
        display->frameViewport(backgroundColor, 1);  // заполним рамку фоновым цветом
        display->resetViewport();                    // сброс окна
    }
    /**
        @brief Установка нового текста метки.

        @param s Текст метки.
        @param redraw Перерисовывать метку.
    */
    void setText(const String& s, bool redraw = true) {
        text = s;
        if (redraw) draw();
    }
    /**
        @brief Установка горизонтального и вертикального отступов (слева и сверху).

        @param h Отступ от левого края.
        @param v Отступ от верхнего края.
        @param redraw Перерисовывать метку.
    */
    void setIndent(uint16_t h, uint16_t v, bool redraw = true) {
        indentH = h;
        indentV = v;
        if (redraw) (draw());
    }
    /**
        @brief Установка иконки, которая будет показана слева от текста.

        @param bitmapptr Указатель на массив данных иконки в формате XBM
        @param w Ширина иконки.
        @param h Высота иконки.
        @param color Цвет иконки.
        @param redraw Перерисовывать метку.
    */
    void setXBitmap(unsigned char* bitmapptr, uint16_t w, uint16_t h, uint32_t color, bool redraw = true) {
        bitmap = bitmapptr;
        bitmapWidth = w;
        bitmapHeight = h;
        bitmapColor = color;
        if (redraw) draw();
    }
    /**
        @brief Установка выравнивания текста
        @param a Выравнивание. 
        @param redraw Перерисовывать метку.
    */
    void setAlign(Align a, bool redraw = true) {
        align = a;
        if (redraw) draw();
    };
  protected:
    uint16_t indentH = 1;   //!< Отступ текста от левого края
    uint16_t indentV = 1;   //!< Отступ текста от верхнего края
    String text = "";       //!< Текст метки
    unsigned char* bitmap;  //!< Указатель на массив данных иконки в формате XBM
    uint32_t bitmapColor;   //!< Цвет иконки.
    uint16_t bitmapWidth;   //!< Ширина иконки.
    uint16_t bitmapHeight;  //!< Высота иконки.
    Align align = alLeft;   //!< Выравнивание текста
};
#endif