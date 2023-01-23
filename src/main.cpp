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
String wochentage[7]={"Sonntag", "Montag", "Dienstag", "Mittwoch", "donnerstag", "Freitag", "Samstag"};

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
#define BUTTON_H 15
ButtonWidget* btn[] = {&btnL, &btnR};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

#include "touch_calibrate.h"
#include "functions.h"


/* --------------------------------------------------------- */






void drawWifiQuality() {
  int8_t quality = getWifiQuality();
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawRightString("  " + String(quality) + "%",305, 5, 1);
  for (int8_t i = 0; i < 4; i++) {
    tft.drawFastVLine(310 + 2 * i,4,8, TFT_LIGHTGREY);
    for (int8_t j = 0; j < 2 * (i + 1); j++) {
      if (quality > i * 25 || j == 0) {
        tft.drawPixel(310 + 2 * i, 12 - j,TFT_BLACK);
      }
    }
  }
}


void getdatetime(){
  time(&now);
  localtime_r(&now, &tm);
  atime=""; adate="";
  adate = wochentage[tm.tm_wday] + "  ";
  if (tm.tm_mday < 10) { adate += "0"; } 
  adate += tm.tm_mday; adate += ".";
  if (tm.tm_mon + 1 < 10){ adate +="0"; }
  adate += tm.tm_mon+1; adate += ".";
  adate += tm.tm_year + 1900;
  if (tm.tm_hour < 10) { atime = "0"; } 
  atime += tm.tm_hour; atime += ":";
  if (tm.tm_min < 10){ atime +="0"; }
  atime += tm.tm_min; atime += ":";
  if (tm.tm_sec < 10){ atime +="0"; }
  atime += tm.tm_sec;
}
  
void ref_page() {
  getdatetime();
  if (oldpage != actpage){
    tft.fillRect(0,15,320,199,TFT_WHITE);
    
    oldpage = actpage;
  }
  displayDateTime();
  switch (actpage) {
  case 1:
    
    break;
  case 2:
    
    break;
  case 3:
    
    break;
  default:
    
    break;
  }
};

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
  timer_clear_status = timerBegin(0,80,true);
  timerAttachInterrupt(timer_clear_status, timer_clear_statusISR, true);
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
  
  getWifiSignal.attach(60, drawWifiQuality);
  pages.attach(10,switch_pages);
  refresh_page.attach(1,ref_page);

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
  uint16_t t_x=9999, t_y=9999;
  //if (millis() - scanTime >= 100 ){
  //  bool pressed = tft.getTouch(&t_x, &t_y);
  //  scanTime=millis();
  //  for (uint8_t b=0; b<buttonCount; b++){
  //    if (pressed){
  //      if (btn[b]->contains(t_x,t_y)){
  //        //btn[b]->press(true);
  //        //btn[b]->pressAction();
  //      }
  //    }
  //    else {
  //      //btn[b]->press(false);
  //      //btn[b]->releaseAction();
  //    } 
  //  } 
  }

