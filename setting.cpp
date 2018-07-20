#include "main_header.h"
/////////////////////////////////////////
void display_setting(uint8_t setting_screen)
{
  
}
void current_time_setting()
{
  
}
void timer_setting()
{
  
}
//////////////////////////////////////////
void setting()
{
  uint8_t setting_selection=1;
  uint8_t last_hour, last_minute,last_second,hour,minute,second;
  get_curent_time(&hour,&minute,&second);
  last_minute=minute;
  last_second=second;
  while(setting_selection!=0)
  {
    display_setting(setting_selection);
    get_current_time(&hour,&minute,&second);
    switch (interrupt_event)
    {
    case UP:
      {
        setting_selection++;
        if(setting_selection>MAX_SETTING_SCREEN) setting_selection=1;
        last_minute=minute;
        last_second=second;
        interrupt_event=NO_EVENT;
      }
    case DOWN:
    {
      setting_selection--;
        if(setting_selection<1) setting_selection=MAX_SETTING_SCREEN;
        last_minute=minute;
        last_second=second;
        interrupt_event=NO_EVENT;
    }
    case ENTER:
    {
      switch(setting_selection)
      {
        case 1:
        {
          current_time_setting();
        }
        case 2:
        {
          timer_setting();
        }
        case 3:
        {
          return;
        }
      }
      last_minute=minute;
      last_second=second;
      interrupt_event=NO_EVENT;
    }
    case NO_EVENT:
    {
      if((((minute-last_minute)*60)+(second-last_second))>return_to_main_screen_time) return;
      display_setting(setting_selection);
    }
   }
  }
}

