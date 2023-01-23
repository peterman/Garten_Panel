#include <Arduino.h>
////////////// More information at: http://www.aeq-web.com?ref=arduinoide ///////////////

//////////////////////// Timer Settings ////////////////////////////////////////////////
int cron_1 = 1;  //5 Seconds intervall for timer 1;
int cron_2 = 5; //20 Seconds intervall for timer 2;
int cron_3 = 30; //30 Seconds intervall for timer 3;
int cron_4 = 60;
int cron_5 = 300;
//////////////////////// End Timer Settings ////////////////////////////////////////////
long cront1, cront2, cront3, cront4, cront5;






void cron1() {
  //Your Cronjob action here
  --timeout_screen;
  if (timeout_screen < 1) {
    timeout_screen = 0;
    bklight = (bklight - (bklight_max - bklight_min)/10);
    if (bklight < bklight_min) { 
      bklight = bklight_min;
      }
  }
}

void cron2() {
  //Your Cronjob action here
  drawWifiQuality();
}

void cron3() {
  //Your Cronjob action here
  //d += 4; if (d > 360) d = 0;

    // Create a Sine wave for testing, value is in range 0 - 100
  //  float value = 50.0 + 50.0 * sin((d + 0) * 0.0174532925);

  //  float current;
  //  current = mapValue(value, (float)0.0, (float)100.0, (float)0.0, (float)2.0);
    //Serial.print("I = "); Serial.print(current);
  //  amps.updateNeedle(current, 0);

  //  float voltage;
  //  voltage = mapValue(value, (float)0.0, (float)100.0, (float)0.0, (float)10.0);
    //Serial.print(", V = "); Serial.println(voltage);
  //  volts.updateNeedle(voltage, 0);
    
  //  float resistance;
  //  resistance = mapValue(value, (float)0.0, (float)100.0, (float)0.0, (float)100.0);
    //Serial.print(", R = "); Serial.println(resistance);
  //  ohms.updateNeedle(resistance, 0);
}

void cron4() {
  //Your Cronjob action here
  
}

void cron5() {
  //Your Cronjob action here
  
}

void cronjob() {
  long tmp = millis();
  if ((cront1 + (cron_1 * 1000)) <= tmp) { cron1(); cront1 = millis(); }
  if ((cront2 + (cron_2 * 1000)) <= tmp) { cron2(); cront2 = millis(); }
  if ((cront3 + (cron_3 * 1000)) <= tmp) { cron3(); cront3 = millis(); }
  if ((cront4 + (cron_4 * 1000)) <= tmp) { cron4(); cront4 = millis(); }
  if ((cront5 + (cron_5 * 1000)) <= tmp) { cron5(); cront5 = millis(); }
}

void IRAM_ATTR timer1ISR(){
  clearStatusBar();
  timerAlarmDisable(timer1);
}