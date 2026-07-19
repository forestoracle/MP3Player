# MP3Player
---
MP3 плеер на основе МК RP2040.

# Зависимости

- **TFT_eSPI** - 
- **GyverEncoder** -
- **SD Card** - 
- **PCM5102** -

# Настройка библиотеки TFT_eSPI

Основные настройки выглядят так

```cpp
#define USER_SETUP_ID 2

#define ST7735_DRIVER
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

#define ST7735_REDTAB
#define TFT_RGB_ORDER TFT_RGB 

#define TFT_CS 15 
#define TFT_DC 16   
#define TFT_RST 17
#define TFT_SDA 19
#define TFT_SCL 18

#define TFT_MOSI 19
#define TFT_SCLK 18

#define LOAD_GLCD   
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6  
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_GFXFF  

#define SMOOTH_FONT

#define SPI_FREQUENCY  27000000

#define SPI_TOUCH_FREQUENCY  2500000
```


# Подключение аппартных компонентов

## Кнопки

|RP2040|Кнопки|Описание|
|------|------|--------|
|      |      |        |

## Энкодер

|RP2040|Энкодер|Описание|
|------|-------|--------|
|      |       |        |

## Дисплей

Дисплей подключен к шине **SPI0**.

Возможные варианты подключения:

- **RX (MISO)**: GPIO 0, 4, 16
- **TX (MOSI)**: GPIO 3, 7, 19
- **SCK (Тактирование)**: GPIO 2, 6, 18
- **CSn (Выбор устройства)**: GPIO 1, 5, 17, 20

Текущее подключение:

|RP2040|Дисплей      |Описание       |
|------|-------------|---------------|
|  15  | CS          | Выбор чипа    |
|  16  | A0/DC       | Данные/команд |
|  17  | RESET       | Сброс         |
|  18  | SCK         | Тактирование  |
|  19  | SDA/MOSI/TX | Данные        |
|  GND | GND         | Земля         |
|  3.3 | VCC         | Питание       |



## SD Card

SD-карта подключена к шине **SPI1**.

Возможные варианты подключения:

- **RX (MISO)**: GPIO 8, 12, 28
- **TX (MOSI)**: GPIO 11, 15
- **SCK (Тактирование)**: GPIO 10, 14
- **CSn (Выбор устройства)**: GPIO 9, 13

Текущее подключение:

|RP2040|SD Card    |Описание|
|------|-----------|--------|
|  12  | MISO (RX) | Приём данных |
|  13  | CS        | Выбор устройства |
|  14  | SCK       | Тактирование |
|  15  | MOSI (TX) | Отсылка данных |

## DAC PCM5102

|RP2040|DAC PCM5102|Описание|
|------|-----------|--------|
|      |           |        |
              

# Шрифты

Для создания шрифтов был использован инструмент из состава библиотеки TFT_eSPI и находящийся 
в каталоге TFT_eSPI\Tools\Create_Smooth_Font. Это скрипт Processing, т.о. для его использования 
необходимо установить систему программирования Processing с сайта [processing.org](processing.org).




