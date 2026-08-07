/**
   @brief MP3 Player
*/
//#define __FREERTOS 1  //  Для использования FreeRTOS нужно определить этот макрос
//#include <FreeRTOS.h>

#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include <SdFat.h>
#include <Wire.h>

#include "PSVision/fonts/verdanab12.h"
#include "PSVision/images/xbm_images.h"
#include "PSVision/progressbar.h"
#include "PSVision/label.h"
#include "PSVision/listbox.h"
#include "PSVision/digitalclock.h"
#include "SDReader.h"
#include "params.h"
#include "TDA7419.h"

#include <BackgroundAudio.h>
#include <I2S.h>
#include "ID3.h"
#include "ID3Callback.h"

#define DEBUG  /// Если этот макрос определён, то будет включена отладка в Serial


#define FASTLED  // Если этот макрос определён, то используем светодиод WS2812 на плате МК. Не на всех платах он есть.
#ifdef FASTLED
#include <FastLED.h>
const pin_size_t WS2812_PIN = 23;     /// пин светодиода WS2812 на плате МК
const uint8_t WS2812_BRIGHTNESS = 1;  /// Яркость светодиода WS2812 на плате МК
#endif

#define FONT_VERDANA_12 verdanab12
/*
    Пин пользовательской кнопки
*/
const pin_size_t USER_BUTTON_PIN = 24;  /// Пользовательская кнопка
const pin_size_t ANALOG_BUTTONS = 29;   /// ADC - пин
const uint16_t ERROR_DELAY = 5000;      /// Длительность показа сообщения об ошибке
/*
    Подключение SD карты к SPI1
*/
const pin_size_t SD_MISO = 12;  /// GPIO12 SPI1 RX
const pin_size_t SD_CS = 13;    /// GPIO13 SPI1 Chip Select
const pin_size_t SD_SCK = 14;   /// GPIO14 SPI1 Clock
const pin_size_t SD_MOSI = 15;  /// GPIO15 SPI1 TX
/*
    Подключение DAC PCM5102 к I2S
*/
const pin_size_t I2S_BCLK = 26;  /// I2S BCK
const pin_size_t I2S_WCLK = 27;  /// I2S LCK
const pin_size_t I2S_DOUT = 28;  /// I2S DIN
/*
    Щина I2C0
*/
const pin_size_t I2C0_SDA = 0;  /// I2C0 SDA
const pin_size_t I2C0_SCL = 1;  /// I2C0 SCL

const uint8_t TDA7419_ADDR = 0x44;  /// адрес аудиопроцессора TDA7419

bool sdInitialized = false;   /// SD-карта проинициализирована
bool bmpInitialized = false;  /// BackgroundAudio проинициализирована
bool tdaInitialized = false;  /// TDA7419 проинициализирована

TFT_eSPI tft = TFT_eSPI();                                      // Дисплей
DigitalClock dc = DigitalClock(0, 0, 160, 128, TFT_RED, &tft);  // Цифровые часы

File root;

SdFat sd;

File f;

I2S audio(OUTPUT);
uint8_t filebuff[512];
BackgroundAudioMP3Class<RawDataBuffer<16 * 1024>> BMP(audio);

//TDA7419 tda;  // Аудиопроцессор

#ifdef FASTLED
CRGB leds[1];
#endif

/*
    Прототипы функций
*/
void splashScreen(String s1, String s2, uint32_t foregroundColor, uint32_t backgroundColor);
bool userButtonPressed(void);
void drawFileList();
void printDebug(String s);

SDReader reader = SDReader();

/********************************************************************** 
 * 
 *  Установочный метод
 * 
**********************************************************************/
void setup() {

#ifdef DEBUG
    /*
      Включение отладки
    */
    Serial.begin(115200);
    delay(1000);  // Это для того, чтобы монитор успел запуститься
    printDebug("\nRasberry Pi Pico MP3 Player\n");
#endif

    // Разгоняем МК до 200 МГц. Нормальная частота - 133 МГц.
    set_sys_clock_khz(200000, true);

    // Включаем 12-битное разрешение для ADC. Эта функция Arduino требует уточнения.
    // Дело в том, что у RP и так 12 бит, но для совместимости Arduino включает 10 бит.
    // И якобы это функция возвращает RP её исходное значение.
    // analogReadResolution(12);

    // Пользовательская кнопка
    pinMode(USER_BUTTON_PIN, INPUT_PULLUP);

#ifdef FASTLED
    // Если есть встроенный в плату WS2812, то зажигаем его красным
    FastLED.addLeds<NEOPIXEL, WS2812_PIN>(leds, 1);
    FastLED.setBrightness(WS2812_BRIGHTNESS);
    leds[0] = CRGB::Red;
    FastLED.show();
#endif
    /*
        Инициализация экрана
    */
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);

    /*
        Инициализация SD-карты на шине SPI1
    */
    SPI1.setRX(SD_MISO);
    SPI1.setTX(SD_MOSI);
    SPI1.setSCK(SD_SCK);
    SPI1.setCS(SD_CS);
    sdInitialized = SD.begin(SD_CS, SPI1);
    if (!sdInitialized) {
        splashScreen("SD-карта", "НЕ инициализирована.", TFT_WHITE, TFT_RED);
        delay(ERROR_DELAY);
    }

#ifdef FASTLED
    leds[0] = sdInitialized ? CRGB::Green1 : CRGB::Red;
    FastLED.show();
#endif


    // Инициализация шины I2C0
    delay(5000);
    Wire.setSDA(I2C0_SDA);
    Wire.setSCL(I2C0_SCL);
    Wire.begin();
    // Пингуем TDA7419
    Wire.beginTransmission(TDA7419_ADDR);
    tdaInitialized = Wire.endTransmission() == 0;
    if (tdaInitialized) {
        printDebug("TDA7419 found");
    } else {
        printDebug("TDA7419 not found!");
        delay(ERROR_DELAY);
    }
    if (tdaInitialized) {
        printDebug("TDA7419 setup");
        TDA7419 tda;
        delay(1000);
        tda.setVolume(1, 1);
        tda.setInput(1, 5, false);
        tda.setAttenuationLeftFront(64 + 1, 0);    
        tda.setAttenuationRightFront(64 + 1, 0);
        tda.setSoftMute(1, 0, 0, 0, 0); 
    }

#ifdef FASTLED
    leds[0] = CRGB::Blue;
    FastLED.show();
#endif
    /*
        Инициализация BackgroudAudio
    */
    bmpInitialized = BMP.begin();
    if (!bmpInitialized) {
        splashScreen("DAC", "не инициализирован.", TFT_WHITE, TFT_RED);
        delay(ERROR_DELAY);
    }
    BMP.setGain(0.1);
    reader.setCurrentDir("/Queen");

    //drawFileList();
#ifdef DEBUG
    printDebug("Выход из setup");
#endif
}  // setup
/********************************************************************** 
 * 
 *  Главный цикл
 * 
***********************************************************************/
void loop() {
    /* if (userButtonPressed()) {
        printDebug("\nКнопка нажата\n");
    }
    delay(200);
    uint16_t analog_buttons = analogRead(ANALOG_BUTTONS);
   
    */
    String name;
    if (!f) {
        //f = SD.open("/Гуляев Юрий/1995 - Любимые мелодии/17 - Имя твоё.mp3");
        reader.nextFile();
        name = reader.getCurrentFileName();
        f = SD.open(name);
        Serial.printf("\r\n\r\nNow playing: %s\r\n", name);
        processID3(f, id3Callback);
        auto p = f.position();
        p = p & ~511;  // Ensure on a sector boundary, MP3 will resync appropriately
        f.seek(p);
    }

    // Stuff the buffer with as much as it will take, only doing full sector reads for performance
    while (f && BMP.availableForWrite() > 512) {
        int len = f.read(filebuff, 512);
        BMP.write(filebuff, len);
        if (len != 512) {
            f.close();  // Short reads == EOF
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
