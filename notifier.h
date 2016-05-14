/* notifier.h
 *
 */
 #ifndef __NOTIFIER_H__
 #define __NOTIFIER_H__

 #include "application.h"
 //#include "util.h"
 #include "parms.h"

 class Notifier {
 public:
     Notifier(char *event_name) {
     //Particle.function("upd", &Notifier::upd, this);
     strcpy(webhook_id, event_name);
     msg_limit = 0;        //limit mesages per hour
#ifdef PUSHOVER_MESSAGES
     alarm_notify_cnt = 0;
     alarm_times_notified = 0;
#endif
     hours_between_alert = ALERT_HOURS;
     digitalWrite(MESSAGE_PIN, LOW);
   }
   //int upd(String command);
   String updData();
#ifdef PUSHOVER_MESSAGES
   void sendMessage(uint8_t priority, char* msg);  //send message to pushover
   void sendMessage(uint8_t, String);
#endif
   void setStartTime();
   void hourlyReset();
   bool checkTime();

 private:
   char webhook_id[20];
   char event_message[1000];   //??reduce size of this ??
   int msg_limit;
#ifdef PUSHOVER_MESSAGES
   int alarm_times_notified;
   int alarm_notify_cnt;
#endif
   int hour = 0;
   int minute = 0;
   int lasthour = 0;   //use -1 to skip startup
   int lastminute = 0;
   int elapsedminutes = 0;
   uint8_t hours_between_alert;
   unsigned long lastSync = millis();
 };
#endif
