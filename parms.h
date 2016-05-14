/* parms.h
 *
 * Configuration parameters
 */
#ifndef __PARMS_H__
#define __PARMS_H__

#define EDT_OFFSET -4
#define EST_OFFSET -5

#define MAX_TEMPERATURE_DIFF 2
#define ALERT_HOURS 1
#define WORRY_MINUTES 4
#define LOOP_DELAY_TIME 100000       // ??Delay ms between loop passes
#define SYSTEM_STATUS_PIN D2        //RED if Armed, Blinking if any tripped
#define SYSTEM_NOTIFICATION_PIN D3  //GREEN blinks at each loop ??
#define TRIGGER_PIN D4              //debug pin for logic analyzer trigger
#define MESSAGE_PIN D7              //on-board led to show message limts reachec
#define EE_MAGIC_CONFIG 0x01        // change this to write new test config
#define EE_MAGIC_STATE 0x01
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
//#define PUSHOVER_MESSAGES           //include Pushover Code
//#define PUSHOVER_SEND_MESSAGES      //actually send Pushover Messages

#endif
