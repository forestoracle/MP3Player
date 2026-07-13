#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "label.h"
#include "images/xbm_images.h"

class Checkbox : public Label {
  public:
    /**
        @brief Конструктор.  

        @param x Координата X левого верхнего угла.
        @param y Координата Y левого верхнего угла.
        @param w Ширина чекбокса в пикселах.
        @param h Высота чекбокса в пикселах.
        @param tft Ссылка на дисплей.
    */
    Checkbox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String s, TFT_eSPI* tft)
        : Label(x, y, w, h, s, tft) {
        bitmapChecked = xbm_checkbox_checked_fat_12x12;
        bitmapUnchecked = xbm_checkbox_unchecked_fat_12x12;
    }
    /**
        @brief Прорисовка чекбокса.  
    */
    void draw(void) override {
        setXBitmap(checked ? bitmapChecked : bitmapUnchecked, bitmapWidth, bitmapHeight, bitmapColor);
        Label::draw();
    }
    /**
        @brief Установка флага checked. 
        @param c Включить или выключить чекбокс.
    */
    void setChecked(bool c) {
        checked = c;
        draw();
    }
    /**
        @brief Инверсия флага checked
    */
    void invert(void) {
        checked = !checked;
        draw();
    }
    /**
        @brief Возврат флага checked
    */
    bool getChecked(void) {
        return checked;
    }
    /**
        @brief Установка изображений для включенного и выключенного чекбокса. 
               Изображение в формате XBM.
        
        @param checked Изображение включенного чекбокса.
        @param unchecked Изображение выключенного чекбокса. 
        @param w Ширина и высота изображений. Предполагается, что они одинаковые, т.е. изображение квадратное.
        @param color Цвет изображения.
    */
    void setCheckBitmaps(unsigned char* checked, unsigned char* unchecked, uint8_t w, uint16_t color) {
        bitmapChecked = checked;
        bitmapUnchecked = unchecked;
        // Предполагается, что изображение квадратное, для чекбокса этого достаточно
        bitmapWidth = w;
        bitmapHeight = w;
        
        bitmapColor = color;
    }
  protected:
    bool checked = false;  //!< Отмечен?
    unsigned char* bitmapChecked; //!< Изображение у включенного чекбокса
    unsigned char* bitmapUnchecked; //!< Изображение у выключенного чекбокса 
};

#endif