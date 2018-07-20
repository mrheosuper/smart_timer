#include "main_header.h"
RTC_DS3231 rtc;
Adafruit_SSD1306 display= Adafruit_SSD1306(4);
uint8_t relay_pin[NUMBER_OF_CHANNEL];
timer_def timer[NUMBER_OF_TIMER];
interrupt_state interrupt_event=NO_EVENT;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void get_current_time(uint8_t * hour, uint8_t * minute, uint8_t * second)
{
  DateTime now=rtc.now();
  *hour=now.hour();
  *minute=now.minute();
  *second=now.second();
}
void check_timer(uint8_t hour, uint8_t minute,uint8_t *state)
{
  for(int i=0;i<NUMBER_OF_CHANNEL;i++)
  {
    state[i]=0;
  }
  for(int i=0;i<NUMBER_OF_TIMER;i++)
  {
    if(timer[i].state==ENABLE&&timer_satisfied(hour,minute,timer[i]))
    {
      state[timer[i].channel]=1;
    }
  }
}
int8_t timer_satisfied(uint8_t hour, uint8_t minute,timer_def timer)
{
  if(timer.begin_hour==timer.stop_hour&&timer.begin_minute==timer.stop_minute&&timer.begin_hour==hour&&timer.begin_minute==minute) return 1;
  if(timer.begin_hour>timer.stop_hour||(timer.begin_hour==timer.stop_hour&&timer.begin_minute>=timer.stop_minute))
  /*
   * 22h30->22h20:right
   * 23h00->21h30:right
   */
  {
    if((hour>timer.begin_hour)||
    (hour==timer.begin_hour&&minute>=timer.begin_minute)||
    (hour<timer.stop_hour)||
    (hour==timer.stop_hour&&minute<=timer.stop_minute))
    return 1;
    else return 0;
    /*
     * ex:22h30->22h20 now is 22h25
     * hour>timer.begin_hour: 22h>22h :wrong
     * (hour==timer.begin_hour&&minute>=timer.begin_minute):22h=22h: right&&25>=30:wrong
     * (hour<timer.stop_hour):22h<22h:wrong
     * hour==timer.stop_hour&&minute<=timer.stop_minute:22h=22h:right&&25<=20:wrong
     * =>return 0
     *
     * ex2:22h30->22h20 now is 22h35
     * hour>timer.begin_hour: 22h>22h :wrong
     * (hour==timer.begin_hour&&minute>=timer.begin_minute):22h=22h:right & &35>=30:right =>return 1
     *
     * ex3:22h30->22h20 now is 22h15
     * hour>timer.begin_hour: 22h>22h :wrong
     * (hour==timer.begin_hour&&minute>=timer.begin_minute):22h=22h:right & &15>=30:wrong
     * (hour<timer.stop_hour):22h<22h:wrong
     *  hour==timer.stop_hour&&minute<=timer.stop_minute:22h=22h:right&&15<=20:right=>return 1
     */
  }
  else
  /*
   * 20h10->22h00 now is 20h20
   */
  {
    if((hour>timer.begin_hour&&hour<timer.stop_hour)||
      (hour==timer.begin_hour&&minute>=timer.begin_minute)||
      (hour==timer.stop_hour&&minute<=timer.stop_minute))
      return 1;
      else return 0;
  }
}
void set_relay(uint8_t state[NUMBER_OF_CHANNEL])
{
 for(int i=0;i<NUMBER_OF_CHANNEL;i++)
 {
  if(state[i]) digitalWrite(relay_pin[i],1);
  else digitalWrite(relay_pin[i],0);
 }
}
void interrupt_handle()
{
  static uint32_t last_interrupt_time=millis();
  if(millis()-last_interrupt_time>=INTERRUPT_INTERVAL)
  {
    last_interrupt_time=millis();
    if(digitalRead(down_pin)==0)
    {
      interrupt_event=DOWN;
      return;
    }
    if(digitalRead(up_pin)==0)
    {
      interrupt_event=UP;
      return;
    }
    interrupt_event=ENTER;
    return;
  }
}
void read_data_from_EEPROM()
{
   for(int i=0;i<NUMBER_OF_TIMER;i++)
    {
    timer[i].begin_hour=EEPROM.read(i*size_of_timer_def);
    timer[i].begin_minute=EEPROM.read((i*size_of_timer_def)+1);
    timer[i].stop_hour=EEPROM.read((i*size_of_timer_def)+2);
    timer[i].stop_minute=EEPROM.read((i*size_of_timer_def)+3);
    timer[i].state=EEPROM.read((i*size_of_timer_def)+4);
    timer[i].channel=EEPROM.read((i*size_of_timer_def)+5);
    }
}
void write_data_to_EEPROM()
{
  for(int i=0;i<NUMBER_OF_TIMER;i++)
  {
    EEPROM.write(i*size_of_timer_def,timer[i].begin_hour);
    EEPROM.write((i*size_of_timer_def)+1,timer[i].begin_minute);
    EEPROM.write((i*size_of_timer_def)+2,timer[i].stop_hour);
    EEPROM.write((i*size_of_timer_def)+3,timer[i].stop_minute);
    EEPROM.write((i*size_of_timer_def)+4,timer[i].state);
    EEPROM.write((i*size_of_timer_def)+5,timer[i].channel);
  }
}
int8_t can_go_to_sleep(uint8_t minute, uint8_t second, uint8_t last_minute, uint8_t last_second)
{
  int16_t temp1=(minute-last_minute);
  temp1=temp1*60;
  int16_t temp2=second-last_second;
  if((temp1+temp2)>go_to_sleep_time) return 1;
  else return 0;
}
void go_to_sleep()
{
  display.clearDisplay();
  display.display();
}
void rtc_error()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(F("Can't start RTC"));
  display.setCursor(0,10);
  display.print(F("Please Reset"));
  display.display();
  while(1);//stop forever here
}
void display_main_screen(uint8_t screen)
{
  
}
///////////////////////MAIN_PROGRAM///////////////////////////////
void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  for(int i=0;i<NUMBER_OF_CHANNEL;i++)
  {
    pinMode(relay_pin[i],OUTPUT);
  }
  pinMode(enter_pin,INPUT_PULLUP);
  pinMode(up_pin,INPUT_PULLUP);
  pinMode(down_pin,INPUT_PULLUP);
  if(!rtc.begin()) rtc_error;
  attachInterrupt(digitalPinToInterrupt(enter_pin),interrupt_handle,FALLING);
  read_data_from_EEPROM();
}
void loop()
{
  uint8_t relay_state[NUMBER_OF_CHANNEL];
  uint8_t hour=0,minute=0,second=0;
  uint8_t main_screen=0;
  get_current_time(&hour,&minute,&second);
  uint8_t last_second=second,last_minute=minute;
  uint8_t is_sleeping=0;
  while(1)
  {
    get_current_time(&hour,&minute,&second);
    check_timer(hour,minute,relay_state);
    set_relay(relay_state);
    switch(interrupt_event)
    {
      case UP:
      {
        main_screen++;
        if(main_screen>=MAX_MAIN_SCREEN) main_screen=0;
        last_second=second;
        last_minute=minute;
        interrupt_event=0;
        is_sleeping=0;
      }
      case DOWN:
      {
        main_screen--;
        if(main_screen<0) main_screen=MAX_MAIN_SCREEN-1;
        last_second=second;
        last_minute=minute;
        interrupt_event=0;
        is_sleeping=0;
      }
      case ENTER:
      {
        setting();
        interrupt_event=0;//NO_EVENT
        last_second=second;
        last_minute=minute;
        is_sleeping=0;
      }
      case NO_EVENT:
      {
        if(can_go_to_sleep(minute,second,last_minute,last_second)) 
        {
          if(is_sleeping==0) 
          {
            go_to_sleep();
            is_sleeping=1;
          }
        }
        else
        {
          display_main_screen(main_screen);
        }
      }
    }
  }
}
















