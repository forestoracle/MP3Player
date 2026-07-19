/**
    @brief Список строк с прокруткой

    Список занимает в высоту и ширину весь экран.
    Список может содержать заголовок и подвал, которые занимают вехнюю и нижнюю позтцтт на экране.
    Строки списка - это набор элементов Label, расположенных вертикально вплотную друг к другу. 
    Их количество определяется высотой экрана в пикселях и наличием (или отсутствием) строк заголовка и подвала.

*/
#ifndef LISTBOX_H
#define LISTBOX_H

#include <TFT_eSPI.h>
#include <stdint.h>
#include "label.h"
#include <vector>
#include <string>
#include "images/xbm_images.h" // Это потом надо убрать как нежелательныю завичимость от конкретного каталога

class Listbox : public Shape {
  public:
    /**
        @brief Конструктор.  

        @param x Координата X левого верхнего угла.
        @param y Координата Y левого верхнего угла.
        @param w Ширина в пикселах.
        @param h Высота в пикселах.
        @param tft Ссылка на дисплей.
    */
    Listbox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, TFT_eSPI* tft)
        : Shape(x, y, w, h, tft),
          header(0, 0, width, tft->fontHeight(), "", display),
          footer(0, y + height - tft->fontHeight() - 1, width, tft->fontHeight(), "", display) {
        createLines();
        //tft->loadFont(verdanab12);
    }
    /**
        @brief Деструктор списка
    */
    ~Listbox() {
        //items.clear();
        // :TODO: Надо разобраться, удаляет ли это сами объекты Label
        //lines.clear();
        display->unloadFont();
    }
    /**
        @brief Полная прорисовка списка
    */
    void draw(void) override {
        //display->loadFont(verdanab12);
        header.draw();
        footer.draw();
        drawLines();
        //display->unloadFont();
    }
    /**
        @brief Установка цветов заголовка списка.

        @param foreground Цвет текста.
        @param background Цвет фона.
        @param redraw Перерисовать список.
    */
    void setHeaderColor(uint32_t foregroud, uint32_t background, bool redraw = true) {
        header.setTextColor(foregroud, background, redraw);
    }
    /**
        @brief Установка текста заголовка списка.
        @param s Текст заголовка.
        @param redraw Перерисовать список.
    */
    void setHeaderText(const String& s, bool redraw = true) {
        header.setText(s, redraw);
    }
    /**
        @brief Установка цветов подвала списка.

        @param foreground Цвет текста.
        @param background Цвет фона.
        @param redraw Перерисовать список.
    */
    void setFooterColor(uint32_t foregroud, uint32_t background, bool redraw = true) {
        footer.setTextColor(foregroud, background, redraw);
    }
    /**
        @brief Установка текста подвала списка.

        @param s Текст подвала.
        @param redraw Перерисовать список.
    */
    void setFooterText(const String& s, bool redraw = true) {
        footer.setText(s, redraw);
    }
    /**
        @brief Установка цветов простого (невыбранного) элемена.
        
        Прорисовка элемента не нужна.
    */
    void setLineColor(uint32_t foreground, uint32_t background) {
        foregroundColor = foreground;
        backgroundColor = background;
    }
    /**
        @brief Установка цветов выбранного элемента.

        Прорисовка элемента не нужна.

        @param foreground Цвет текста.
        @param background Цвет фона.
    */
    void setSelectedColor(uint32_t foreground, uint32_t background) {
        selectedForegroundColor = foreground;
        selectedBackgroundColor = background;
    }
    /**
        @brief Выбрать следующий элемент.

        После выбора предыдущего элемента itemIndex увеличивается на 1.
        Если перед вызовом был выбран itemCount-1 элемент, то ничего не происходит и itemIndex остаётся прежним.
    */
    uint16_t selectNext(void) {
        // Выбран последний элемент - выходим
        if (itemIndex == items.size() - 1) return itemIndex;
        itemIndex++;
        // Выбранный элемент находится в последней строке
        if (itemIndex == firstItem + lines.size()) {
            firstItem++;  // увеличиваем индекс элемента в первой строке - скроллинг списка вниз
        }
        drawLines();  // перерисовываем строки списка
        return itemIndex;
    }
    /**
        @brief Выбрать предыдущий элемент.

        После выбора предыдущего элемента itemIndex уменьшается на 1.
        Если перед вызовом был выбран 0-й элемент, то ничего не происходит и itemIndex остаётся прежним.
    */
    uint16_t selectPrior(void) {
        // Выбран первый элемент - выходим
        if (itemIndex == 0) return itemIndex;
        if (itemIndex == firstItem) {
            firstItem--;  // увеличиваем индекс элемента в первой строке - скроллинг списка вниз
        }
        itemIndex--;  // уменьшаем индекс элемента в первой строке - скроллинг списка вверх
        drawLines();  // перерисовываем строки списка
        return itemIndex;
    }
    /**
        @brief Выбрать первый элемент.

        Нумерация начинается с 0.
        Если перед вызовом был выбран 0-й элемент, то ничего не происходит и itemIndex остаётся прежним.
    */
    uint16_t selectFirst(void) {
        if (itemIndex == 0) return itemIndex;
        itemIndex = 0;
        firstItem = 0;
        drawLines();
        return itemIndex;
    }
    /**
        @brief Выбрать последний элемент.

        Нумерация заканчивается itemCount - 1
        Если перед вызовом был выбран itemCount - 1 элемент, то ничего не происходит и itemIndex остаётся прежним.
    */
    uint16_t selectLast(void) {
        if (itemIndex == items.size() - 1) return itemIndex;
        itemIndex = items.size() - 1;
        firstItem = items.size() - lines.size();
        drawLines();
        return itemIndex;
    }

    /**
        @brief Возвращает индекс выбранного элемента.

        Нумерация начинается с 0, заканчивается items.size() - 1
    */
    uint16_t getItemIndex(void) {
        return itemIndex;
    }
    /**
        @brief Возвращает количество элеменов в списке.
    */
    uint16_t getItemCount(void) {
        return items.size();
    }
    /**
        @brief Добавляет элемент в конец списка.
        @param s Строка для добавления.
        @return Возвращает его индекс. Нумерация начинается с 0.
    */
    void addListItem(String s) {
        // Начальное значение индекса выбранного элемента = 0xFFFF, т.е. ничего не выбрано.
        // При добавлении первого элемента в список индекс выбранного устанавливается в 0
        if (itemIndex == 0xFFFF) itemIndex = 0;
        // Добавляем строку в конец списка
        items.push_back(s);
    }
    /**
        @brief Создание списка строк списка. 
    */
    void createLines(void) {
        uint16_t beginY = header.getHeight();  // с этой точки начинаем прорисовывать элемены списка
        // Вычисляем количество элементов списка
        uint8_t linesCount = (display->height() - footer.getHeight() - beginY) / display->fontHeight();

        for (uint8_t count = 0; count < linesCount; count++) {
            Label l = Label(0, 1 + beginY + display->fontHeight() * count, display->width(), display->fontHeight() + 2, "", display);
            l.setIndent(0, 0);
            lines.push_back(l);
        }
    }
    /**
        @brief Прорисовка элементов списка.
    */
    void drawLines(void) {
        for (uint16_t count = 0; count < lines.size(); count++) {
            lines[count].setText(items[firstItem + count]);
            // Обычные элементы списка рисуются отлично от выбранного
            if (itemIndex == firstItem + count) {
                lines[count].setTextColor(selectedForegroundColor, selectedBackgroundColor);
            } else {
                lines[count].setTextColor(foregroundColor, backgroundColor);
            }
            lines[count].draw();
        }
    }

    Label* getLine(uint16_t index) {
        if (index > lines.size() - 1) return nullptr;
        return &(lines[index]);
    }    
  private:
    uint16_t firstItem = 0;       //!< индекс элемента, показанного в первой строке
    uint16_t itemIndex = 0xFFFF;  //!< индекс выбранного элемента
    uint16_t lineIndex = 0;       //!< индекс выбранной строки

    uint32_t selectedForegroundColor = TFT_BLACK; //!< Цвет текста выбранного элемена 
    uint32_t selectedBackgroundColor = TFT_WHITE; //!< Цвет фона выбранного элемена

    Label header;               //!< Заголовок списка
    Label footer;               //!< Подвал списка
    std::vector<Label> lines;   //!< Это элементы списка, выводимые на экран
    std::vector<String> items;  //!< Содержимое списка - строки
};
#endif