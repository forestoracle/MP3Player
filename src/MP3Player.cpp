/**
   @brief MP3 Player

*/
//#define __FREERTOS 1  //  Для использования FreeRTOS нужно определить этот макрос
//#include <FreeRTOS.h>

#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include "PSVision/fonts/verdanab12.h"
#include "PSVision/images/xbm_images.h"
#include "PSVision/progressbar.h"
#include "PSVision/label.h"
#include "PSVision/listbox.h"
#include "PSVision/digitalclock.h"

#define DEBUG      //!< Если этот макрос определён, то будет включена отладка в Serial
#define FONT_VERDANA_12 verdanab12

#define USER_BUTTON_PIN 24  // Пин пользовательской кнопки

/*
    Подключение SD карты к SPI1
*/
const pin_size_t SD_MISO = 12;  // GPIO12 SPI1 RX
const pin_size_t SD_CS = 13;    // GPIO13 SPI1 Chip Select
const pin_size_t SD_SCK = 14;   // GPIO14 SPI1 Clock
const pin_size_t SD_MOSI = 15;  // GPIO15 SPI1 TX

bool sdInitialized = false;  // SD-карта проинициализирована

TFT_eSPI tft = TFT_eSPI();
DigitalClock dc = DigitalClock(0, 0, 160, 128, TFT_RED, &tft);
File root;

/*
    Прототипы функций
*/
void splashScreen(String s1, String s2, uint32_t foregroundColor, uint32_t backgroundColor);
bool userButtonPressed(void);
bool initSDCard(void);
void drawFileList();
void printDebug(String s);
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
    delay(5000);  // Это для того, чтобы монитор успел запуститься
#endif
    printDebug("Вход в setup");
    printDebug("\nRasberry Pi Pico MP3 Player\n");

    pinMode(USER_BUTTON_PIN, INPUT_PULLUP);  // Пользовательская кнопка


    /*
    *  Инициализация экрана
    */
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    /*
        Инициализация SD-карты
    */
    splashScreen("Инициализация", "SD-карты.", TFT_WHITE, TFT_BLUE);
    sdInitialized = initSDCard();
    if (!sdInitialized) {
        splashScreen("SD-карта", "НЕ инициализирована.", TFT_WHITE, TFT_RED);
    } else {
        splashScreen("SD-карта", "инициализирована.", TFT_WHITE, TFT_RED);
    }
    drawFileList();
    printDebug("Выход из setup");
}  // setup
/********************************************************************** 
 * 
 *  Главный цикл
 * 
*/
void loop() {
    if (userButtonPressed()) {
        printDebug("\nКнопка нажата\n");
    }
    delay(200);
}  // loop

/**
    @brief  Нажата ли пользовательская кнопка
*/
bool userButtonPressed(void) {
    return digitalRead(USER_BUTTON_PIN) == LOW;
}  // userButtonPressed

/**
    @brief Экран с сообщением.
*/
void splashScreen(String s1, String s2, uint32_t foregroundColor, uint32_t backgroundColor) {
    tft.fillScreen(backgroundColor);
    tft.loadFont(FONT_VERDANA_12);
    Label l1(0, 50, 160, 15, s1, &tft);
    l1.setTextColor(foregroundColor, backgroundColor, false);
    l1.setAlign(alCenter);
    Label l2(0, 65, 160, 15, s2, &tft);
    l2.setTextColor(foregroundColor, backgroundColor, false);
    l2.setAlign(alCenter);
    tft.unloadFont();
} // splashScreen

void printDirectory(File dir, int numTabs) {
    while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
            // no more files
            break;
        }
        for (int i = 0; i < numTabs; i++) {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.print(entry.size(), DEC);
            time_t cr = entry.getCreationTime();
            time_t lw = entry.getLastWrite();
            struct tm tmstruct;
            localtime_r(&cr, &tmstruct);
            Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
            localtime_r(&lw, &tmstruct);
            Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
        }
        entry.close();
    }
}
/*
    Инициализация шины SPI1 для SD-карты
*/
void initSPI1(void) {
    SPI1.setRX(SD_MISO);
    SPI1.setTX(SD_MOSI);
    SPI1.setSCK(SD_SCK);
}  // initSPI1
/*
    Инициализация SD-карты.
*/
bool initSDCard(void) {
    initSPI1();
    if (!SD.begin(SD_CS, SPI1)) {
        return false;
    }
    return true;
}

/*
    Прорисока списка
*/
void drawFileList() {
    tft.loadFont(verdanab12);
    Listbox lb = Listbox(0, 0, tft.width(), tft.height(), &tft);
    lb.setHeaderColor(TFT_BLUE, TFT_WHITE, false);
    lb.setFooterColor(TFT_WHITE, TFT_BLUE, false);
    lb.setHeaderText("/", false);
    lb.setFooterText("Подвал 1", false);
    lb.setLineColor(TFT_BLACK, TFT_DARKGREY);
    lb.setSelectedColor(TFT_WHITE, TFT_BLUE);
    
    root = SD.open("/");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            // нет больше файлов
            break;
        }
        if (entry.isDirectory()) {
            lb.addListItem(String(entry.name()));
        }    
    }
    lb.draw();
    tft.unloadFont();
}

void printDebug(String s) {
#ifdef DEBUG
    Serial.println(s);
#endif
}
