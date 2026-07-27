/**
   @brief MP3 Player

*/
//#define __FREERTOS 1  //  Для использования FreeRTOS нужно определить этот макрос
//#include <FreeRTOS.h>

#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include <SdFat.h>
#include "PSVision/fonts/verdanab12.h"
#include "PSVision/images/xbm_images.h"
#include "PSVision/progressbar.h"
#include "PSVision/label.h"
#include "PSVision/listbox.h"
#include "PSVision/digitalclock.h"
#include "SDReader.h"

#include <BackgroundAudio.h>
#include <I2S.h>
#include "ID3.h"
#include "ID3Callback.h"

File f;
I2S audio(OUTPUT);
uint8_t filebuff[512];
// We will make a larger buffer because SD cards can sometime take a long time to read
BackgroundAudioMP3Class<RawDataBuffer<16 * 1024>> BMP(audio);

#define DEBUG  //!< Если этот макрос определён, то будет включена отладка в Serial
#define FONT_VERDANA_12 verdanab12
/*
    Пин пользовательской кнопки
*/
const pin_size_t USER_BUTTON_PIN = 24;
/*
    Подключение SD карты к SPI1
*/
const pin_size_t SD_MISO = 12;  // GPIO12 SPI1 RX
const pin_size_t SD_CS = 13;    // GPIO13 SPI1 Chip Select
const pin_size_t SD_SCK = 14;   // GPIO14 SPI1 Clock
const pin_size_t SD_MOSI = 15;  // GPIO15 SPI1 TX
/*
    Подключение I2S
*/
const pin_size_t PIN_BCLK = 26;  // BCK
const pin_size_t PIN_WCLK = 27;  // LCK
const pin_size_t PIN_DOUT = 28;  // DIN

bool sdInitialized = false;  // SD-карта проинициализирована

/// @brief Дисплей
TFT_eSPI tft = TFT_eSPI();

/// @brief Цифровые часы
DigitalClock dc = DigitalClock(0, 0, 160, 128, TFT_RED, &tft);

File root;

SdFat sd;

/*
    Прототипы функций
*/
void splashScreen(String s1, String s2, uint32_t foregroundColor, uint32_t backgroundColor);
bool userButtonPressed(void);
bool initSDCard(void);
void drawFileList();
void printDebug(String s);
void initSPI1(void);


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
    delay(10000);  // Это для того, чтобы монитор успел запуститься
#endif
    // Разгон процессора для стабильного декодирования MP3 без хрипов
    set_sys_clock_khz(200000, true);  // 200 MHz

    uint actual_baud = spi_get_baudrate(spi1);
    printDebug(String(actual_baud));

    actual_baud = spi_set_baudrate(spi1, 15000000);
    printDebug(String(actual_baud));

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
    initSPI1();
    splashScreen("Инициализация", "SD-карты.", TFT_BLACK, TFT_WHITE);
    sdInitialized = initSDCard();
    if (!sdInitialized) {
        splashScreen("SD-карта", "НЕ инициализирована.", TFT_WHITE, TFT_RED);
    } else {
        splashScreen("SD-карта", "инициализирована.", TFT_WHITE, TFT_BLUE);
    }

    //drawFileList();

    printDebug("Выход из setup");
}  // setup
/********************************************************************** 
 * 
 *  Главный цикл
 * 
*/
void loop() {
    /* if (userButtonPressed()) {
        printDebug("\nКнопка нажата\n");
    }
    delay(200);
    */
if (!f) {
    f = SD.open("/demo-192.mp3");
    Serial.printf("\r\n\r\nNow playing: %s\r\n", "demo-192.mp3");
    processID3(f, id3Callback);
    auto p = f.position();
    p = p & ~511; // Ensure on a sector boundary, MP3 will resync appropriately
    f.seek(p);
  }

  // Stuff the buffer with as much as it will take, only doing full sector reads for performance
  while (f && BMP.availableForWrite() > 512) {
    int len = f.read(filebuff, 512);
    BMP.write(filebuff, len);
    if (len != 512) {
      f.close(); // Short reads == EOF
    }
  }

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
}  // splashScreen

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
    SPI1.setCS(SD_CS);
}  // initSPI1

/*
    Инициализация SD-карты.
*/
bool initSDCard(void) {
    if (!SD.begin(SD_CS, SPI1)) {
        return false;
    }
    return true;
}


/*
    Прорисока списка
*/
void drawFileList() {
    SDReader reader;

    tft.loadFont(verdanab12);
    Listbox lb = Listbox(0, 0, tft.width(), tft.height(), &tft);
    lb.setHeaderColor(TFT_BLUE, TFT_WHITE, false);
    lb.setFooterColor(TFT_WHITE, TFT_BLUE, false);
    lb.setHeaderText("/", false);
    lb.setFooterText("Подвал 1", false);
    lb.setLineColor(TFT_BLACK, TFT_DARKGREY);
    lb.setSelectedColor(TFT_WHITE, TFT_BLUE);

    root = SD.open("/Толкунова Валентина");
    // 1.01. Я не могу иначе.mp3
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }
        printDebug(String(entry.name()));
    }
    lb.draw();
    tft.unloadFont();
}

void printDebug(String s) {
#ifdef DEBUG
    Serial.println(s);
#endif
}
