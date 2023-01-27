#include <Arduino.h>

/* Löschen Interrupt Statusline löschen */
void IRAM_ATTR timer_clear_statusISR (){
  timerAlarmDisable(timer_clear_status);
}


/* Button Reaktionen  */
void btnL_pressAction(void){
    if (btnL.justPressed()){
        actpage--; if (actpage < 0) { actpage = 3; }
        Serial.println("Left Button");
    }
}

void btnL_releaseAction(void){

}
void btnR_pressAction(void){
    if (btnR.justPressed()){
        actpage++; if (actpage > 3) { actpage = 0; }
        Serial.println("Right Button");
    }
}

void btnR_releaseAction(void){

}


/* initialisieren der Buttons */
void initButtons(){
  uint16_t x = (tft.width() - BUTTON_W);
  uint16_t y = (tft.height()- BUTTON_H);
  btnL.initButtonUL(0,y,BUTTON_W,BUTTON_H,TFT_BLACK,TFT_LIGHTGREY,TFT_BLUE,"<-",1);
  btnL.setLabelDatum(-12,1,0);
  btnL.drawButton(false,"<-");
  btnL.setPressAction(btnL_pressAction);
  btnL.setReleaseAction(btnL_releaseAction);
  
  btnR.initButtonUL(x,y,BUTTON_W,BUTTON_H,TFT_BLACK,TFT_LIGHTGREY,TFT_BLUE,"->",1);
  btnR.setLabelDatum(0,1,0);
  btnR.drawButton(false,"->");
  btnR.setPressAction(btnR_pressAction);
  btnR.setReleaseAction(btnR_releaseAction);
}

/* Lösche obere und untere Statusleiste */
void clear_top_bar(){
  tft.fillRect(0,0,320,15,TFT_LIGHTGREY);
  tft.drawRect(0,0,320,15,TFT_BLACK);
}




/* converts the dBm to a range between 0 and 100% */
int8_t getWifiQuality() {
  if (WiFi.status() != WL_CONNECTED){
    return -1;
  } else {
    int32_t dbm = WiFi.RSSI();
    if(dbm <= -100) {
        return 0;
    } else if(dbm >= -50) {
        return 100;
    } else {
        return 2 * (dbm + 100);
    }
  }
}


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
