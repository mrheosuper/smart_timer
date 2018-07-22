#include "main_header.h"
void display_setting_screen(int8_t setting)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(23,0);
  display.print(F("SETTING"));
  display.setTextSize(1);
  display.setCursor(8,19);
  display.print(F("Change time"));
  display.setCursor(8,28);
  display.print(F("Timer Setting"));
  display.setCursor(8,37);
  display.print(F("Return main screen"));
  display.setCursor(0,19+setting*9);
  display.print(F(">"));
  display.display();
}
void display_change_time_setting(int8_t screen,int8_t mode, int8_t time[3])
{
  display.clearDisplay();
  display.setTextSize(1);
  if(mode)
  {
    display.setCursor(8,screen*9);
    switch(screen)
    {
      case 0:
      {
      display.print(F("HOUR: "));
      display.print(time[0]);
      break;
      }
      case 1:
      {
      display.print(F("MINUTE: "));
      display.print(time[1]);
      break;
      }
      case 2:
      {
      display.print(F("SECOND: "));
      display.print(time[2]);
      break;
      }
    }
  }
  else
  {
    display.setCursor(8,0);
    display.print(F("HOUR: "));
    display.print(time[0]);
    display.setCursor(8,9);
    display.print(F("MINUTE: "));
    display.print(time[1]);
    display.setCursor(8,18);
    display.print(F("SECOND: "));
    display.print(time[2]);
    display.setCursor(8,27);
    display.print(F("Apply"));
    display.setCursor(8,36);
    display.print(F("Return"));
  }
  display.setCursor(0,screen*9);
  display.print(F(">"));
  display.display();
}
void change_current_time()
{
  interrupt_event=NO_EVENT;
  int8_t hour=0,minute=0,second=0;
  get_current_time(&hour,&minute,&second);
  int8_t last_hour=hour, last_minute=minute,last_second=second;
  int8_t setting_time=0;
  int8_t time_to_change[3]={hour,minute,second};
  int8_t is_changing_time=0;
  DateTime now=rtc.now();
  while(1)
  {
    get_current_time(&hour,&minute,&second);
    if(time_to_change[0]>23) time_to_change[0]=0;
    if(time_to_change[0]<0) time_to_change[0]=23;
    if(time_to_change[1]>59) time_to_change[1]=0;
    if(time_to_change[1]<0) time_to_change[1]=59;
    if(time_to_change[2]>59) time_to_change[2]=0;
    if(time_to_change[2]<0) time_to_change[2]=59;
    switch(interrupt_event)
    {
      case UP:
      {
        if(is_changing_time==0)
        {
        setting_time++;
        if(setting_time>=MAX_SETTING_TIME_SCREEN) setting_time=0;
        }
        else
        {
          time_to_change[setting_time]++;
        }
        interrupt_event=NO_EVENT;
        last_minute=minute;
        last_second=second;
        break;
      }
      case DOWN:
      {
        if(is_changing_time==0)
        {
        setting_time--;
        if(setting_time<0) setting_time=MAX_SETTING_TIME_SCREEN-1;
        }
        else
        {
          time_to_change[setting_time]--;
        }
        interrupt_event=NO_EVENT;
        last_minute=minute;
        last_second=second;
        break;
      }
      case ENTER:
      {
        if(setting_time==MAX_SETTING_TIME_SCREEN-1&&is_changing_time!=1) return;
        if(setting_time==MAX_SETTING_TIME_SCREEN-2&&is_changing_time!=1)
          {
            now=rtc.now();
            rtc.adjust(DateTime(now.year(),now.month(),now.day(),time_to_change[0],time_to_change[1],time_to_change[2]));
            return;
          }
        else 
        {
          is_changing_time=!is_changing_time;
          interrupt_event=NO_EVENT;
          last_minute=minute;
          last_second=second;
          break;
        }
      }
      case NO_EVENT:
      {
       if(can_go_to_sleep(minute,second,last_minute,last_second)) return;
       else display_change_time_setting(setting_time,is_changing_time,time_to_change);
       break;
      }
    }
  }
}
void display_set_timer_parameter(uint8_t parameter,uint8_t timer_parameter[size_of_timer_def],int8_t is_changing);
void display_timer_selection(int8_t timer_choosing)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print(F("Timer:"));
  display.print(timer_choosing);
  display.display();
}
void set_timer_parameter(int8_t choosing)
{
  interrupt_event=NO_EVENT;
  int8_t hour=0,minute=0,second=0;
  get_current_time(&hour,&minute,&second);
  int8_t last_hour=hour, last_minute=minute,last_second=second;
  DateTime now=rtc.now();
  int8_t parameter=0;
  int8_t is_changing=0;
  int8_t parameter_of_timer[size_of_timer_def];
  parameter_of_timer[0]=timer[choosing].begin_hour;
  parameter_of_timer[1]=timer[choosing].begin_minute;
  parameter_of_timer[2]=timer[choosing].stop_hour;
  parameter_of_timer[3]=timer[choosing].stop_minute;
  parameter_of_timer[4]=timer[choosing].state;
  parameter_of_timer[5]=timer[choosing].channel;
  while(1)
  {
    get_current_time(&hour,&minute,&second);
    if(parameter_of_timer[0]>23) parameter_of_timer[0]=0;
    if(parameter_of_timer[0]<0) parameter_of_timer[0]=23;
    if(parameter_of_timer[2]>23) parameter_of_timer[2]=0;
    if(parameter_of_timer[2]<0) parameter_of_timer[2]=23;
    if(parameter_of_timer[1]>59) parameter_of_timer[1]=0;
    if(parameter_of_timer[1]<0) parameter_of_timer[1]=59;
    if(parameter_of_timer[3]>59) parameter_of_timer[3]=0;
    if(parameter_of_timer[3]<0) parameter_of_timer[3]=59;
    if(parameter_of_timer[4]>1) parameter_of_timer[4]=0;
    if(parameter_of_timer[4]<0) parameter_of_timer[4]=1;
    if(parameter_of_timer[5]>=NUMBER_OF_CHANNEL) parameter_of_timer[5]=0;
    if(parameter_of_timer[5]<0) parameter_of_timer[5]=NUMBER_OF_CHANNEL-1;
    switch(interrupt_event)
    {
      case NO_EVENT:
      {
        if(can_go_to_sleep(minute,second,last_minute,last_second)) return;
        else {
          display_set_timer_parameter(parameter,parameter_of_timer,is_changing);
          //Serial.println("display_Set_timer_parameter");
        }
        break;
      }
      case ENTER:
      {
        //Serial.println(parameter);
        if(parameter==(size_of_timer_def)) 
        {
          timer[choosing].begin_hour=parameter_of_timer[0];
          timer[choosing].begin_minute=parameter_of_timer[1];
          timer[choosing].stop_hour=parameter_of_timer[2];
          timer[choosing].stop_minute=parameter_of_timer[3];
          timer[choosing].state=parameter_of_timer[4];
          timer[choosing].channel=parameter_of_timer[5];
          write_data_to_EEPROM();
          interrupt_event=NO_EVENT;
          return;
          break;
        }
        if(parameter==(size_of_timer_def+1))
        {
          interrupt_event=NO_EVENT;
          return;
          break;
        }
        is_changing=!is_changing;
        last_minute=minute;
        last_second=second;
        interrupt_event=0;
        break;
      }
      case UP:
      {
        if(is_changing) parameter_of_timer[parameter]++;
        else
        {
        parameter++;
        if(parameter>=size_of_timer_def+2) parameter=0;
        }
        last_minute=minute;
        last_second=second;
        interrupt_event=0;
        break;
      }
      case DOWN:
      {
        if(is_changing) parameter_of_timer[parameter]--;
        else
        {
        parameter--;
        if(parameter<0) parameter=size_of_timer_def+1;
        }
        last_minute=minute;
        last_second=second;
        interrupt_event=0;
        break;
      }
    }
  }
}
void display_set_timer_parameter(uint8_t parameter,uint8_t timer_parameter[size_of_timer_def],int8_t is_changing)
{
  //Serial.println(parameter);
  const char * text[size_of_timer_def+2]={"BEGIN_HOUR:","BEGIN_MINUTE:","STOP_HOUR:","STOP_MINUTE:","STATE:","CHANNEL:","Apply","Return"};
  display.clearDisplay();
  display.setTextSize(1);
  if(is_changing)
  {
    display.setCursor(8,parameter*8);
    display.print(text[parameter]);
    if(parameter!=4)
    {
      display.print(timer_parameter[parameter]);
    }
    else
    {
      if(timer_parameter[parameter]) display.print(F("ACTIVE"));
      else display.print(F("DEACTIVE"));
    }
  }
  else
  {
    for(int i=0;i<size_of_timer_def+2;i++)
    {
      display.setCursor(8,i*8);
      display.print(text[i]);
      if(i==4)
      {
        if(timer_parameter[i])display.print(F("ACTIVE"));
        else display.print(F("DEACTIVE"));
      }
      else
      {
      if(i<size_of_timer_def)display.print(timer_parameter[i]);
      }
    }
  }
  display.setCursor(0,parameter*8);
  display.print(F(">"));
  display.display();
}
void timer_setting()
{
  interrupt_event=NO_EVENT;
  int8_t hour=0,minute=0,second=0;
  get_current_time(&hour,&minute,&second);
  int8_t last_hour=hour, last_minute=minute,last_second=second;
  int8_t choosing_timer=0;
  DateTime now=rtc.now();
  while(1)
  {
    get_current_time(&hour,&minute,&second);
    switch(interrupt_event)
    {
      case NO_EVENT:
      {
        if(can_go_to_sleep(minute,second,last_minute,last_second)) return;
        else display_timer_selection(choosing_timer);
        break;
      }
      case UP:
      {
        choosing_timer++;
        if (choosing_timer>=NUMBER_OF_TIMER) choosing_timer=0;
        interrupt_event=NO_EVENT;
        last_minute=minute;
        last_hour=hour;
        break;
      }
      case DOWN:
      {
        choosing_timer--;
        if(choosing_timer<0) choosing_timer=NUMBER_OF_TIMER-1;
        interrupt_event=NO_EVENT;
        last_minute=minute;
        last_hour=hour;
        break;
      }
      case ENTER:
      {
        set_timer_parameter(choosing_timer);
        return;
        break;
      }
    }
  }
}
void setting()
{
  interrupt_event=NO_EVENT;
  int8_t hour=0,minute=0,second=0;
  get_current_time(&hour,&minute,&second);
  int8_t last_hour=hour, last_minute=minute,last_second=second;
  int8_t is_setting=1;
  int8_t setting_mode=0;
  while(is_setting)
  {
    get_current_time(&hour,&minute,&second);
    switch(interrupt_event)
    {
      case UP:
      {
        setting_mode++;
        if(setting_mode>=MAX_SETTING_SCREEN) setting_mode=0;
        interrupt_event=NO_EVENT;
        last_minute=minute;
        last_second=second;
        break;
      }
      case DOWN:
      {
        setting_mode--;
        if(setting_mode<0) setting_mode=MAX_SETTING_SCREEN-1;
        interrupt_event=NO_EVENT;
        last_minute=minute;
        last_second=second;
        break;
      }
      case ENTER:
      {
        switch(setting_mode)
        {
          case 0:
          {
            change_current_time();
            last_minute=minute;
            last_second=second;
            break;
          }
          case 1:
          {
            timer_setting();
            last_minute=minute;
            last_second=second;
            break;
          }
          case 2:
          {
            return;
            break;
          }
        }
        interrupt_event=NO_EVENT;
        break;
      }
      case NO_EVENT:
      {
        if(can_go_to_sleep(minute,second,last_minute,last_second)) return;
        else display_setting_screen(setting_mode);
        break;
      }
    }
  }
}

