#include <Arduino.h>

/* Löschen Interrupt Statusline löschen */
void IRAM_ATTR timer_clear_statusISR (){
  timerAlarmDisable(timer_clear_status);
}


/* Button Reaktionen  */
void btnL_pressAction(void){
    if (btnL.justPressed()){
        actpage++; if (actpage > 3) { actpage = 0; }
    }
}

void btnL_releaseAction(void){

}
void btnR_pressAction(void){
    if (btnL.justPressed()){
        actpage--; if (actpage < 0) { actpage = 3; }
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

void clear_status_bar(){
  tft.fillRect(30,224,260,239,TFT_LIGHTGREY);
  tft.drawRect(30,224,260,239,TFT_BLACK);
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

/* schaltet seiten um */
void switch_pages(){
  actpage++; if (actpage > 3) { actpage = 0; }
  
  Serial.println(String(actpage));
}