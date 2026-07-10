/**
   @brief MP3 Player

*/
//#define __FREERTOS 1  //  Для использования FreeRTOS нужно определить этот макрос
//#include <FreeRTOS.h>

#include <TFT_eSPI.h>
#include <SPI.h>

//#include "fonts/verdana12.h"
#include "PSVision/fonts/verdanab12.h"
#include "PSVision/progressbar.h"
#include "PSVision/label.h"
#include "PSVision/listbox.h"
#include "PSVision/digitalclock.h"
#include <RTC.h>



#define DEBUG      //!< Если этот макрос определён, то будет включена отладка в Serial
#define TFT_BL 15  // Backlite
#define FONT_VERDANA_12 verdanab12

TFT_eSPI tft = TFT_eSPI();
DigitalClock dc = DigitalClock(0, 0, 160, 128, TFT_RED, &tft);
datetime_t t;

//ProgressBar pb1 = ProgressBar(10, 1, 100, 10, 0, 100, tft);
//ProgressBar pb2 = ProgressBar(10, 12, 100, 10, 0, 100, tft);
//ProgressBar pb3 = ProgressBar(10, 24, 100, 10, 0, 100, tft);
//ProgressBar pb4 = ProgressBar(10, 36, 100, 10, 0, 100, tft);
//ProgressBar pb5 = ProgressBar(10, 48, 100, 10, 0, 100, tft);

void printDebug(String s) {
#ifdef DEBUG
    Serial.println(s);
#endif
}

const uint USER_BUTTON_PIN = 24;
/**
    @brief  Нажата ли пользовательская кнопка
*/
bool userButtonPressed(void) {
    return digitalRead(USER_BUTTON_PIN) == LOW;
}
/*
    Прорисока списка
*/
void drawFileList() {
    //tft.loadFont(verdanab12);
    Listbox lb = Listbox(0, 0, tft.width(), tft.height(), &tft);
    for (uint16_t s = 0; s < 100; s++) {
        lb.addListItem(String(s));
    }
    lb.setHeaderColor(TFT_BLUE, TFT_WHITE, false);
    lb.setFooterColor(TFT_WHITE, TFT_BLUE, false);
    lb.setHeaderText("Заголовок 1", false);
    lb.setFooterText("Подвал 1", false);
    lb.setLineColor(TFT_BLACK, TFT_DARKGREY);
    lb.setSelectedColor(TFT_WHITE, TFT_BLUE);
    lb.draw();
    // Тест
    for (uint16_t i = 0; i < 15; i++) {
        delay(500);
        lb.selectNext();
    }
    for (uint16_t i = 0; i < 16; i++) {
        delay(500);
        lb.selectPrior();
    }
    for (uint16_t i = 0; i < 15; i++) {
        delay(500);
        lb.selectNext();
    }
    lb.selectFirst();
    delay(1000);
    lb.selectLast();
    //tft.unloadFont();
}

void randomBars(void) {
    /*
    pb1.setValue(random(100));
    pb2.setValue(random(100));
    pb3.setValue(random(100));
    pb4.setValue(random(100));
    pb5.setValue(random(100));
    */
}  // randomBars

/********************************************************************** 
 * 
 *  Установочный метод
 * 
*/
void setup() {

#ifdef DEBUG
    /*
      Включение отладки
    */
    Serial.begin(115200);
#endif

    printDebug("\nRasberry Pi Pico MP3 Player\n");

    pinMode(USER_BUTTON_PIN, INPUT_PULLUP);  // Пользовательская кнопка


    /*
    *  Инициализация экрана
    */
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);

    dc.setTime(11, 34, 34);
    dc.setTextColor(TFT_WHITE, TFT_BLACK);
    _rtc_init();
    t.day = 1;
    t.hour =12;
    t.min = 3;
    t.sec = 4;
    t.year = 2026;
    rtc_set_datetime(&t);
    //drawFileList();
}

/********************************************************************** 
 * 
 *  Главный цикл
 * 
*/
void loop() {
    if (userButtonPressed()) {
#ifdef DEBUG
        Serial.print("\nКнопка нажата\n");
        Serial.println(dc.getTimeHH24MI());
#endif
        rtc_get_datetime(&t);
        dc.setTime(t.hour, t.min, t.sec);
        dc.draw();
    }
    delay(200);
}
