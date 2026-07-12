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
            imageChecked = xbm_checkbox_checked_fat_12x12;
            imageUnchecked = xbm_checkbox_unchecked_fat_12x12;
    }
    /**
        @brief Установка флага checked 
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
    
  protected:
    bool checked = false;  //!< Отмечен?
    unsigned char* imageChecked;
    unsigned char* imageUnchecked;

};

#endif