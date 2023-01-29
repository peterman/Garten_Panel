#include <Arduino.h>

////////////// More information at: http://www.aeq-web.com?ref=arduinoide ///////////////

//////////////////////// Timer Settings ////////////////////////////////////////////////
int cron_1 = 1;  //5 Seconds intervall for timer 1;
int cron_2 = 5; //20 Seconds intervall for timer 2;
int cron_3 = 30; //30 Seconds intervall for timer 3;
int cron_4 = 60;
int cron_5 = 60;
//////////////////////// End Timer Settings ////////////////////////////////////////////
long cront1, cront2, cront3, cront4, cront5;
bool cron1_e, cron2_e, cron3_e, cron4_e, cron5_e = 0;





void cron1() {
  //Your Cronjob action here
  getdatetime();
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawString(adate + " / " + atime + "   ", 10, 5, 1); 
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  
  if (oldpage != actpage){
    tft.fillRect(0,15,320,199,TFT_WHITE);
    
    oldpage = actpage;
  }
  
  switch (actpage) {
  case 1: 
    tft.drawCentreString("Page 1",160,120,2);
    break;
  case 2:
    tft.drawCentreString("Page 2",160,120,2);
    break;
  case 3:
    tft.drawCentreString("Page 3",160,120,2);
    break;
  default:
    
    break;
  }
}

void cron2() {
  //Your Cronjob action here
  drawWifiQuality();
}

void cron3() {
  //Your Cronjob action here
  actpage++; if (actpage > 3) { actpage = 0; }
  
}

void cron4() {
  //Your Cronjob action here
  
}

void cron5() {
  //Your Cronjob action here
  for (int i = 0; i <= 210; i++){
    ledcWrite(0,i);
    delay(10);
  }
  
  cron5_e = 0 ;
}

void cronjob() {
  long tmp = millis();
  if ((cron1_e == 0)) {cront1 = millis();}
  if ((cron1_e == 1) && (cront1 + (cron_1 * 1000)) <= tmp) { cron1(); cront1 = millis(); }
  if ((cron2_e == 0)) {cront2 = millis();}
  if ((cron2_e == 1) && (cront2 + (cron_2 * 1000)) <= tmp) { cron2(); cront2 = millis(); }
  if ((cron3_e == 0)) {cront3 = millis();}
  if ((cron3_e == 1) && (cront3 + (cron_3 * 1000)) <= tmp) { cron3(); cront3 = millis(); }
  if ((cron4_e == 0)) {cront4 = millis();}
  if ((cron4_e == 1) && (cront4 + (cron_4 * 1000)) <= tmp) { cron4(); cront4 = millis(); }
  if ((cron5_e == 0)) {cront5 = millis();}
  if ((cron5_e == 1) && (cront5 + (cron_5 * 1000)) <= tmp) { cron5(); cront5 = millis(); }
}

