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
//#include <ESPConnect.h>
#include <LittleFS.h>
#include <PubSubClient.h>


#include <time.h>
#include <Preferences.h>

#define GFXFF 1
#define FF18 &FreeSans12pt7b




#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define LED0 2
#define BKLED 15
#define BEEPER 26
#define TFT_CAL_FILE "/.touchdata"
#define REPEAT_CAL false
#define DEFAULT_WIFI_TIMEOUT 60      // 60 Sekunden
#define SPIFFS LittleFS

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
#define BUTTON_W 30
#define BUTTON_H 20
ButtonWidget* btn[] = {&btnL, &btnR};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

#include "touch_calibrate.h"
#include "functions.h"
#include "crontab.h"

/* ------------------------------------------------------------------------------------ */


void check_first_boot(){
}

bool connect_WiFi(){
    unsigned long timer;
    char ssid[33];     // Maximallänge: 32
    char wifipwd[64];  // Maximallänge: 63
    char hostname[64]; // Maximallänge: 63
    int wifi_timeout;

    

    if ((wificonfig.getString("ssid", ssid, 32) == 0) || (wificonfig.getString("wifipwd", wifipwd, 32) == 0)) {
      return false;
    }
    
    wifi_timeout = wificonfig.getInt("timeout", DEFAULT_WIFI_TIMEOUT);
    if (wificonfig.isKey("hostname")) {
      wificonfig.getString("hostname", hostname, 63);
      WiFi.setHostname(hostname);
    }
    // mit dem WLAN verbinden
    tft.print("Verbindung herstellen mit "); tft.println(ssid);
    timer = millis() / 1000;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifipwd);
    while (WiFi.status() != WL_CONNECTED && (millis() / 1000) < timer + wifi_timeout) {
      delay(500);
      tft.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
      tft.println("\nTimeout beim Verbindungsaufbau!");
      return false;
    } else {
      tft.print("\nVerbunden mit IP: ");
      tft.println(WiFi.localIP());
      return true;
    }
}






  

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  tft.drawCentreString(String(info.wifi_sta_disconnected.reason),160,120,2);
  Serial.println("Trying to Reconnect");
  //WiFi.begin(ssid, password);
}
/* ------------------------------------------------------------------------------------ */


void setup() {
  //  disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);
  // Interrupt Clear Status
  timer_clear_status = timerBegin(0,80,true);
  timerAttachInterrupt(timer_clear_status, timer_clear_statusISR, true);
  // set Display blanked
  pinMode(BKLED, OUTPUT);
  digitalWrite(BKLED,1);

  Serial.begin(115200);
  wificonfig.begin("wifi", false);
  version.begin("version", false);

  check_first_boot();
  
  tft.init();
  tft.setRotation(1);
  digitalWrite(BKLED,0);
  
  touch_calibrate();
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
    
  tft.setViewport(50,30,220,160);
  tft.frameViewport(TFT_SKYBLUE, -2);
  tft.setViewport(55,35,210,150);
  tft.setCursor(0,10,1);

  if (connect_WiFi()) {
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  } else {
    tft.println("Starte Access-Point");
    WiFi.softAP("ESP32-Portal", NULL);
    IPAddress IP = WiFi.softAPIP();
    tft.print("AP-IP address: ");
    tft.println(IP);
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("setupwifi.html");
    server.on("/",HTTP_POST, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()){
          if (p->name() == "ssid"){
            wificonfig.putString("ssid", p->value().c_str());
          }
          if (p->name() == "pass"){
            wificonfig.putString("wifipwd", p->value().c_str());
          }
        }
      }
    request->send(200, "text/plain", "done. ESP will restart.");
    tft.println("Config erledigt. ESP32 wird neu gestartet.");
    delay(3000);
    ESP.restart();
    });
  }
  delay(5000);
  tft.resetViewport();
  clear_top_bar(); //clear_status_bar();
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  tft.drawCentreString("hole Zeitserver",160,140,2);

  // starte NTP
  configTime(0,0,NTP_SERVER);
  setenv("TZ",MY_TZ,1);
  tzset();


  

  
  
  
  
  // starte Server
  
    
  server.addHandler(new SPIFFSEditor(SPIFFS, "admin","admin"));
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
  
  
  

  Serial.println(WiFi.localIP().toString()); 
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  //tft.drawString(ESPConnect.getSSID(),10,5,1);
  tft.drawCentreString(WiFi.localIP().toString(),100,225,1);
  initButtons();
  drawWifiQuality();
  
  
  
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

