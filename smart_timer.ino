#include "main_header.h"
RTC_DS3231 rtc;
Adafruit_SSD1306 display= Adafruit_SSD1306(4);
uint8_t relay_pin[NUMBER_OF_CHANNEL]={5,6,7,8,9};
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
  //Serial.print("Now:");Serial.print(hour);Serial.print(":");Serial.println(minute);
  //Serial.print("Begin:");Serial.print(timer.begin_hour);Serial.print(":");Serial.println(timer.begin_minute);
  //Serial.print("Stop:");Serial.print(timer.stop_hour);Serial.print(":");Serial.println(timer.stop_minute);
  int32_t delta_hour=timer.stop_hour-timer.begin_hour;
  delta_hour*=60;
  int32_t delta_minute=timer.stop_minute-timer.begin_minute;
  if(delta_hour+delta_minute>=0)
  {
  if(hour<=timer.stop_hour&&hour>=timer.begin_hour&&minute<=timer.stop_minute&&minute>=timer.begin_minute) return 1;
  else return 0;
  }
  else
  {
    if((hour>=timer.begin_hour&&minute>=timer.begin_minute)||(hour<=timer.stop_hour&&minute<=timer.stop_minute)) return 1;
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
  static uint64_t last_interrupt_time=millis();
  if(abs(millis()-last_interrupt_time)>=INTERRUPT_INTERVAL)
  {
    //Serial.print("Up/Down");Serial.print(digitalRead(up_pin));Serial.println(digitalRead(down_pin)); //for debugging
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
void welcome_screen()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(F("Smart Timer"));
  display.setCursor(0,16);
  display.print(F("By PhanHaiBac"));
  display.display();
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
void display_main_screen(uint8_t screen,uint8_t hour, uint8_t minute, uint8_t second, uint8_t relay_state[NUMBER_OF_CHANNEL])
{
  switch(screen)
  {
   case 0:
   { 
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(5,20);
    if(hour<10) display.print(F("0")); 
    display.print(hour); 
    if(second%2) display.print(F(":"));
    else display.print(F(" "));
    if(minute<10) display.print(F("0"));
    display.print(minute);
    display.display();
    break;
   }
   case 1:
   {
    display.clearDisplay();
    display.setTextSize(1);
    for(int i=0;i<NUMBER_OF_CHANNEL;i++)
    {
      display.setCursor(0,i*8);
      display.print(F("Channel "));
      display.print(i);
      display.print(F(": "));
      if(relay_state[i]==1) display.print(F("ON"));
      else display.print(F("OFF"));
    }
    display.display();
    break;
   }
   default:
   {
    display.clearDisplay();
    display.setTextSize(1);
    int8_t temp=0;
    int8_t pos=0;
    if((timer_per_main_screen*(screen-1))>=NUMBER_OF_TIMER) temp=NUMBER_OF_TIMER;
    else temp=(timer_per_main_screen*(screen-1));
    for(int i=(timer_per_main_screen*(screen-2));i<temp;i++)
    {
      display.setCursor(0,pos*8);
      display.print(F("timer "));
      display.print(i);
      if(timer[i].state==DISABLE) display.print(F(": DISABLED"));
      else
      {
      display.print(F(": "));
      display.print(timer[i].begin_hour);
      display.print(F(":"));
      display.print(timer[i].begin_minute);
      display.print(F("->"));
      display.print(timer[i].stop_hour);
      display.print(F(":"));
      display.print(timer[i].stop_minute);     
      }
      pos++;
    }
    display.display();
    break;
   }
  }
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
  welcome_screen();
  delay(2000);
  //while(1);
  //Serial.begin(9600);
  //timer[1].state=ENABLE;
}
void loop()
{
  uint8_t relay_state[NUMBER_OF_CHANNEL];
  uint8_t hour=0,minute=0,second=0;
  int8_t main_screen=0;
  get_current_time(&hour,&minute,&second);
  uint8_t last_second=second,last_minute=minute;
  uint8_t is_sleeping=0;
  while(1)
  {
    get_current_time(&hour,&minute,&second);
    check_timer(hour,minute,relay_state);
    set_relay(relay_state);
    //relay_state[2]=1;
    switch(interrupt_event)
    {
      case UP:
      {
        if(is_sleeping)
        {
          main_screen=0;
          is_sleeping=0;
          last_second=second;                               //After sleeping we want to display home screen
          last_minute=minute;
          interrupt_event=0;
        }
        else
        {
        main_screen++;
        if(main_screen>=MAX_MAIN_SCREEN) main_screen=0;
        last_second=second;
        last_minute=minute;
        interrupt_event=0;
        is_sleeping=0;
        }
        break;
      }
      case DOWN:
      {
        if(is_sleeping)
        {
          main_screen=0;
          is_sleeping=0;
          last_second=second;                           //Same reason
          last_minute=minute;
          interrupt_event=0;
        }
        else
        {
        main_screen--;
        if(main_screen<0) main_screen=MAX_MAIN_SCREEN-1;
        last_second=second;
        last_minute=minute;
        interrupt_event=0;
        is_sleeping=0;
        }
        break;
      }
      case ENTER:
      {
        if(is_sleeping)
        {
          main_screen=0;
          is_sleeping=0;
          last_second=second;                                 //Same reason
          last_minute=minute;
          interrupt_event=0;
        }
        else
        {
        setting();
        interrupt_event=0;//NO_EVENT
        last_second=second;
        last_minute=minute;
        is_sleeping=0;
        main_screen=0;//after setting go to main screen
        }
        break;
      }
      case NO_EVENT:
      {
        if(can_go_to_sleep(minute,second,last_minute,last_second)||is_sleeping) 
        {
          if(is_sleeping==0) 
          {
            go_to_sleep();
            is_sleeping=1;
          }
          main_screen=0;// go to main screen after sleeping
        }
        else
        {
          display_main_screen(main_screen,hour,minute,second,relay_state);
        }
        //Serial.print(freeMemory());Serial.println("Byte");
        break;
      }
    }
  }
}
















