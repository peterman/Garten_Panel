#include <Arduino.h>

#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define LED0 2
#define BKLED 15
#define BEEPER 26
#define TFT_CAL_FILE "/.touchdata"
#define REPEAT_CAL false
#define DEFAULT_WIFI_TIMEOUT 60      // 60 Sekunden
#define FS LittleFS
#define BUTTON_W 30
#define BUTTON_H 20

hw_timer_t * timer_clear_status = NULL;
time_t now;
struct tm tm;
String adate, atime;
String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

uint32_t scanTime;

int actpage = 0, oldpage = 99;

TFT_eSPI tft=TFT_eSPI();


Preferences wificonfig;
Preferences version;
AsyncWebServer server(80);

ButtonWidget btnL = ButtonWidget(&tft);
ButtonWidget btnR = ButtonWidget(&tft);

ButtonWidget* btn[] = {&btnL, &btnR};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);
