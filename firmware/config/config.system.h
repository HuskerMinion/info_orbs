#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#undef ILI9341_DRIVER
#define GC9A01_DRIVER

#define TFT_SDA_READ

// Default pin values
// Pins can be overridden in platformio.ini build_flags section e.g. -D TFT_MOSI=23
#ifndef TFT_MOSI
    #define TFT_MOSI 17
#endif
#ifndef TFT_MISO
    #define TFT_MISO -1
#endif
#ifndef TFT_SCLK
    #define TFT_SCLK 23
#endif
#ifndef TFT_CS
    #define TFT_CS -1
#endif
#ifndef TFT_DC
    #define TFT_DC 19
#endif
#ifndef TFT_RST
    #define TFT_RST 18
#endif

#ifndef SCREEN_1_CS
    #define SCREEN_1_CS 13
#endif
#ifndef SCREEN_2_CS
    #define SCREEN_2_CS 33
#endif
#ifndef SCREEN_3_CS
    #define SCREEN_3_CS 32
#endif
#ifndef SCREEN_4_CS
    #define SCREEN_4_CS 25
#endif
#ifndef SCREEN_5_CS
    #define SCREEN_5_CS 21
#endif

#ifndef BUTTON_LEFT_PIN
    #define BUTTON_LEFT_PIN 26
#endif
#ifndef BUTTON_MIDDLE_PIN
    #define BUTTON_MIDDLE_PIN 27
#endif
#ifndef BUTTON_RIGHT_PIN
    #define BUTTON_RIGHT_PIN 14
#endif

#ifndef BUTTON_DEBOUNCE_TIME
    #define BUTTON_DEBOUNCE_TIME 35 // Debounce buttons for X ms
#endif

#ifndef BUTTON_MEDIUM_PRESS_TIME
    #define BUTTON_MEDIUM_PRESS_TIME 500 // Medium press is registered after X ms
#endif

#ifndef BUTTON_LONG_PRESS_TIME
    #define BUTTON_LONG_PRESS_TIME 2000 // Long press is registered after X ms
#endif

#ifndef BUTTON_MODE
    #define BUTTON_MODE INPUT_PULLDOWN
#endif

#ifndef BUSY_PIN
    #define BUSY_PIN 2
#endif

#ifndef NTP_SERVER
    #define NTP_SERVER "pool.ntp.org"
#endif

#ifndef SCREEN_SIZE
    #define SCREEN_SIZE 240
#endif
#ifndef TFT_WIDTH
    #define TFT_WIDTH SCREEN_SIZE
#endif
#ifndef TFT_HEIGHT
    #define TFT_HEIGHT SCREEN_SIZE
#endif

// Legacy fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8N
#define LOAD_GFXFF
#define SMOOTH_FONT

#ifndef SPI_FREQUENCY
    #define SPI_FREQUENCY 27000000
#endif

#ifndef TIMEZONE_API_URL
    #define TIMEZONE_API_URL "https://timeapi.io/api/timezone/zone"
#endif

#ifndef MAX_RETRIES
    #define MAX_RETRIES 3
#endif

#endif
