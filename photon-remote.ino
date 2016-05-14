/*
  remote alarm system node for Particle Photon
  my car dog temperature alarm
  @file     Alarm-Remote.ino
  @author   device_list. Thompson
  @license  GNU General Public License (see license.txt)
  @version  0.0.1

  Copyright (C) 2016 Donald Thompson and Raynham Engineering

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define SYSTEM_VERSION 0.0.1
#define SYSTEM_VERSION_MAJOR 0
#define SYSTEM_VERSION_MINOR 0
#define SYSTEM_VERSION_SUB 1

#define SERIAL_DEBUG
//#define SERIAL_WAIT

#include "application.h"
#include "Adafruit_MCP9808.h"
#include "notifier.h"
#include "parms.h"
#include "math.h"

int ledStatus = SYSTEM_STATUS_PIN;            //blink this LED at each loop pass
int ledNotice = SYSTEM_NOTIFICATION_PIN;      //blink this LED at each notification
int ledStatusState = 0;
int ledStatusBlink = 0;                       // 0=solid,1=slow blink,2=fast blink
int ledStatusBlinkCount = 0;
float cur_temperature;                        //temperature just read
float prev_temperature;                       //temperature last reported
//float max_setting;                            //read from cloud in setup()??

Adafruit_MCP9808 mcp9808;

#include "state.h"
State sys;
Notifier notify("remote-msg");

void setup() {
  pinMode(ledStatus, OUTPUT);
  pinMode(ledNotice, OUTPUT);
  sys.sysState(sys_undefined);
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
  #ifdef SERIAL_WAIT
    while(!Serial.available())  // Wait here until the user presses ENTER
      Spark.process();          // in the Serial Terminal. Call the BG Tasks
    Serial.read();
  #endif
#endif
#ifdef SERIAL_DEBUG
    delay(1000);
    Serial.printf("\\nPhotonRemote initializing...Version %d.%d.%d",
          SYSTEM_VERSION_MAJOR, SYSTEM_VERSION_MINOR, SYSTEM_VERSION_SUB);
#endif
    if (Particle.connected() == false) {
      Particle.connect();
    }
    sys.sysState(sys_starting);
    Time.zone(EDT_OFFSET);
    uint8_t stat = mcp9808.begin(MCP9808_I2CADDR);  //bus master
    if(stat) {
      sys.addStatus(fail_hardware);
      sys.sysState(sys_fail); //this will do HANG
    }
    if(sys.sysStatus()==0) {
      sys.sysState(sys_running);
    }
    notify.setStartTime();
#ifdef SERIAL_DEBUG
    Serial.println("running...");
#endif
    cur_temperature = mcp9808.readTempF();
    prev_temperature = 0.0;
    digitalWrite(MESSAGE_PIN, HIGH);       //blink red led on publish
}

void Notifier::hourlyReset() {
  msg_limit = 0;
  digitalWrite(MESSAGE_PIN, LOW);
  if (millis() - lastSync > ONE_DAY_MILLIS) {
    // Request time synchronization from the Particle Cloud
    Particle.syncTime();
    lastSync = millis();
  }
}

// set time at initialization
void Notifier::setStartTime() {
  hours_between_alert = 1;
  hour=Time.hour();
  lasthour = hour - (hour % hours_between_alert);
  if(lasthour <0) lasthour += 24;
#ifdef SERIAL_DEBUG
    Serial.print("hour=");
    Serial.println(hour);
    Serial.print("lasthour=");
    Serial.println(lasthour);
#endif
    lastminute=Time.minute();
}

bool Notifier::checkTime() {
  String worry_message;
  bool do_check = FALSE;
  int tempHour = hour;
  int min_hours_between;
  hour=Time.hour();
  minute=Time.minute();

  // 10  18  18-10 = 8
  elapsedminutes = minute - lastminute;
  //50 10 10 - 50 + 60 = 20
  if(elapsedminutes < 0) elapsedminutes += 60;
  if(elapsedminutes >= WORRY_MINUTES) {
    lastminute = minute;
    do_check = TRUE;
  }
  min_hours_between = max(1,hours_between_alert);
  tempHour = hour;
  if(tempHour < lasthour) tempHour += 24;
  //if hours_between_alert = 4  {4,8,12}
  //FIRST TIME EARLY: LAST_HOUR 3
  //if hours_between_alert is zero, don't alert but do log temp hourly
  if(tempHour - lasthour >= min_hours_between) { //NEEDS TO BE ON BOUNDARY
    if(lasthour!=-1){
#ifdef PUSHOVER_MESSAGES
      if(hours_between_alert != 0) {
         worry_message = updData();
         sendMessage(worry_message);
      }
#endif
      //char tempF[10];
      //sprintf(tempF,"%4.1fF",p_sensor->readTemperature());
      //Particle.publish("temperature2", tempF);
      hourlyReset();
      do_check = TRUE;
      #ifdef SERIAL_DEBUG
        Serial.print("hour changed! ");
        Serial.print("HOUR:");
        Serial.println(hour);
      #endif
    }
    lasthour = hour;
  }
}
// format common update message
String Notifier::updData() {
  char uptime[40];
  char buf[40];
  //make this a method in Notifier
  //to share with worry
  sys.upTime(uptime);
  String msg = String("\\nsys up time: ");
  msg.concat(uptime);
  sprintf(buf,"\\nworry %d hours",hours_between_alert);
  msg.concat(buf);
  return msg;
}

void loop() {
  char data[10];
  cur_temperature = mcp9808.readTempF();
  bool do_temp_send = notify.checkTime();

  //if((abs(cur_temperature - prev_temperature) > MAX_TEMPERATURE_DIFF) ||
  //  (cur_temperature >= max_setting && prev_temperature < max_setting)) {
  if(abs(cur_temperature - prev_temperature) > MAX_TEMPERATURE_DIFF)
    do_temp_send = TRUE;
  if(do_temp_send) {
    sprintf(data,"%f",cur_temperature);
    //can we just publish the float ??
    Particle.publish("remotetemp", data);
    prev_temperature = cur_temperature;
    digitalWrite(MESSAGE_PIN, HIGH);       //blink red led on publish
    delay(LOOP_DELAY_TIME); //use two delays to show led
  }
  else {
    delay(LOOP_DELAY_TIME); //use two delays to show led
    digitalWrite(MESSAGE_PIN, LOW);
  }
  //Particle.variable("temp", temperatureF);
  //Particle.variable("state", stateA);
#ifdef SERIAL_DEBUG
  Serial.print(".");
#endif
  delay(LOOP_DELAY_TIME); //use two delays to show led
}
