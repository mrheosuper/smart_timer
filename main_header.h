//#ifndef main_header_h
#define main_head_h
#include <EEPROM.h>
#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
//////////////////////
#define NUMBER_OF_CHANNEL 5
#define NUMBER_OF_TIMER 10
#define size_of_timer_def 6     // number of byte of timer_def required
#define ENABLE 1                //ACTIVE_state =1
#define DISABLE 0
#define enter_pin 2            //Pin for button
#define down_pin 3
#define up_pin 4
#define go_to_sleep_time 30
#define return_to_main_screen_time 60
#define MAX_MAIN_SCREEN 4
#define MAX_SETTING_SCREEN 3
#define INTERRUPT_INTERVAL 100
#define size_of_timer_def 6
typedef struct
{
  uint8_t begin_hour=00;
  uint8_t begin_minute=00;
  uint8_t stop_hour=00;
  uint8_t stop_minute=00;
  uint8_t state=DISABLE;
  uint8_t channel=0;
  //boolean invert_state;
} timer_def;
typedef enum  {
  NO_EVENT,
  ENTER,
  UP,
  DOWN
} interrupt_state;
extern interrupt_state interrupt_event;
extern timer_def timer[NUMBER_OF_TIMER];
extern Adafruit_SSD1306 display;
extern void get_curent_time(uint8_t * hour, uint8_t * minute, uint8_t * second);
void setting();
//#endif
