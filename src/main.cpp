#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ESPConnect.h>
#include <LittleFS.h>
#include <PubSubClient.h>

#include <Ticker.h>
#include <time.h>

#define GFXFF 1
#define FF18 &FreeSans12pt7b




#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define LED0 2
#define BKLED 15
#define BEEPER 26
#define TFT_CAL_FILE "/.touchdata"
#define REPEAT_CAL false
#define SPIFFS LittleFS

hw_timer_t * timer_clear_status = NULL;
time_t now;
tm tm;
String adate, atime;
String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

Ticker getWifiSignal;
Ticker pages;
Ticker refresh_page;
uint32_t scanTime;

int actpage = 0, oldpage = 99;


TFT_eSPI tft=TFT_eSPI();



AsyncWebServer server(80);

ButtonWidget btnL = ButtonWidget(&tft);
ButtonWidget btnR = ButtonWidget(&tft);
#define BUTTON_W 30
#define BUTTON_H 20
ButtonWidget* btn[] = {&btnL, &btnR};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

#include "touch_calibrate.h"
#include "functions.h"
#include "crontab.h"

/* --------------------------------------------------------- */









  

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  tft.drawCentreString(String(info.wifi_sta_disconnected.reason),160,120,2);
  Serial.println("Trying to Reconnect");
  //WiFi.begin(ssid, password);
}
/* --------------------------------------------------------- */


void setup() {
  //  disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);
  // Interrupt Clear Status
  //timer_clear_status = timerBegin(0,80,true);
  //timerAttachInterrupt(timer_clear_status, timer_clear_statusISR, true);
  pinMode(BKLED, OUTPUT);
  digitalWrite(BKLED,0);

  Serial.begin(115200);
  Serial.println("Starting...");

  

  tft.init();
  tft.setRotation(1);
  

  touch_calibrate();
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  clear_top_bar(); //clear_status_bar();
  // starte NTP
  configTime(0,0,NTP_SERVER);
  setenv("TZ",MY_TZ,1);
  tzset();
  
  // starte Server
  ESPConnect.autoConnect("ESP32Config");
  ESPConnect.begin(&server);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    
  server.addHandler(new SPIFFSEditor(SPIFFS, "admin","admin"));
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
  
  
  
  //refresh_page.attach(1,ref_page);

  Serial.println(WiFi.localIP().toString()); 
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  tft.drawString(ESPConnect.getSSID(),10,5,1);
  tft.drawString(WiFi.localIP().toString(),100,5,1);
  initButtons();
  drawWifiQuality();
  digitalWrite(BKLED,0);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t scanTime = millis();
  uint16_t t_x=0, t_y=0;
  if (millis() - scanTime >= 50 ){
    bool pressed = tft.getTouch(&t_x, &t_y);
    scanTime=millis();
    for (uint8_t b=0; b<buttonCount; b++){
      if (pressed){
        if (btn[b]->contains(t_x,t_y)){
          btn[b]->press(true);
          btn[b]->pressAction();
        }
      }
      else {
        btn[b]->press(false);
        btn[b]->releaseAction();
      } 
    }
  } 
  cronjob();  
  delay(10);
}

