#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <TimeLib.h>
#include <TimerOne.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define CLOCK 0
#define MENU 1
#define SET_TIME 2
#define SET_ALARM 3
#define ALARM_TRIGGERED 4

//some values
int state = CLOCK, setValues[] = {0,0,0}, setAlarm[] = {0,0,0}, set_pos = 0;
bool Alarm_State = false, Menu_Option = true;

void setup() {
  //initialise time / lcd / timer1 / timer1.interrupt
  setTime(0, 0, 0, 0, 0, 0);
  lcd.begin(16, 2);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(checkAlarmTime);
}

void loop() {
 uint8_t buttons = lcd.readButtons();
  
 switch (state){

  //shows current time in real time and whether the alarm is on/off
  case CLOCK:
    //display
    lcd.setCursor(0,0);
    if (Alarm_State == true) {
      lcd.print("-!-");
      lcd.setCursor(13,0);
      lcd.print("-!-");
    }
    else {
      lcd.print("---");
      lcd.setCursor(13,0);
      lcd.print("---");
    }
    lcd.setCursor(4,0);
    Cur_Time();
    lcd.setCursor(0,1);
    lcd.print("Menu = (Select)");
    if (buttons & BUTTON_SELECT) {lcd.clear(); state = MENU;}
  break;

  //menu, display and navigation for set time / set alarm
  case MENU:
    //display
    lcd.setCursor(0,0);
    if (Menu_Option == true){
      lcd.print("1. Set Time  <--");
    }
    else{
      lcd.print("1. Set Time"); 
    }
    lcd.setCursor(0,1);
    if (Menu_Option == true){
      lcd.print("2. Set Alarm");
    }
    else{
      lcd.print("2. Set Alarm <--"); 
    }
    if (buttons & BUTTON_UP) {lcd.clear(); Menu_Option = true;}
    if (buttons & BUTTON_DOWN) {lcd.clear(); Menu_Option = false;}
    if (buttons & BUTTON_SELECT) {
      lcd.clear();
      if (Menu_Option == true) {
        state = SET_TIME;
      }
      else {
        state = SET_ALARM;
      }
    }    
  break;
  
  case SET_TIME:
    //display
    //print the current values stored in setAlarm[] to the screen
    lcd.setCursor(4,0);
    checkDigits(setValues[0]);
    lcd.print(":");
    checkDigits(setValues[1]); 
    lcd.print(":");
    checkDigits(setValues[2]);

    //print text to bottom line of screen
    lcd.setCursor(1,1);
    lcd.print("Set = (SELECT)");

    //block of code which enables cursor to show user which value is currently being edited
    lcd.cursor();
    if (set_pos == 0) {
      lcd.setCursor(5,0);
    }
    else if (set_pos == 1) {
      lcd.setCursor(8,0);  
    }
    else {
      lcd.setCursor(11,0);      
    }

    //deals with up/down buttons
    //allows user to choose values for currently selected option hr(0-24)/min(0-59)/sec(0-59)
    if (set_pos == 0) {
      if (buttons & BUTTON_UP){
        if (setValues[set_pos] == 23) {
          setValues[set_pos] = 0;
        }
        else {
          setValues[set_pos]++;
        }
      }  
      if (buttons & BUTTON_DOWN){
        if (setValues[set_pos] == 0) {
          setValues[set_pos] = 23;
        }
        else {
          setValues[set_pos]--;
        }
      }
    }
    else {
      if (buttons & BUTTON_UP){
        if (setValues[set_pos] == 59) {
          setValues[set_pos] = 0;
        }
        else {
          setValues[set_pos]++;
        }
      }
      if (buttons & BUTTON_DOWN){
        if (setValues[set_pos] == 0) {
          setValues[set_pos] = 59;
        }
        else {
          setValues[set_pos]--;
        }
      }
    }    

    //deals with left/right buttons
    //allows user to select between hr/min/sec to be edited
    if (buttons & BUTTON_LEFT) {
      if (set_pos == 0) {
        set_pos = 2;
      }
      else {
        set_pos--;
      }
    }
    if (buttons & BUTTON_RIGHT) {
      if (set_pos == 2) {
        set_pos = 0;
      }
      else {
        set_pos++;
      }
    }

    //change state to CLOCK if BUTTON_SELECT pressed (sets time with values in setValues[])
    if (buttons & BUTTON_SELECT) {
      setTime(setValues[0],setValues[1],setValues[2],0,0,0);
      set_pos = 0;
      lcd.noCursor();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Time set");
      delay(1000);
      lcd.clear();
      state = CLOCK;}
  break;

  //lets user set an alarm time and toggle on/off
  case SET_ALARM:
    //display
    //print the current values stored in setValues[] to the screen
    lcd.setCursor(4,0);
    checkDigits(setAlarm[0]);
    lcd.print(":");
    checkDigits(setAlarm[1]); 
    lcd.print(":");
    checkDigits(setAlarm[2]);
    lcd.setCursor(13,0);
    if (Alarm_State) {
      lcd.print("ON!");
    }
    else {
      lcd.print("OFF");
    }

    //print text to bottom line of screen
    lcd.setCursor(1,1);
    lcd.print("Set = (SELECT)");

    //block of code which enables cursor to show user which value is currently being edited
    lcd.cursor();
    if (set_pos == 0) {
      lcd.setCursor(5,0);
    }
    else if (set_pos == 1) {
      lcd.setCursor(8,0);  
    }
    else if (set_pos == 2) {
      lcd.setCursor(11,0);      
    }
    else {
      lcd.setCursor(13,0);
    }

    //deals with up/down buttons
    //allows user to choose values for currently selected option hr(0-24)/min(0-59)/sec(0-59)
    if (set_pos == 0) {
      if (buttons & BUTTON_UP){
        if (setAlarm[set_pos] == 23) {
          setAlarm[set_pos] = 0;
        }
        else {
          setAlarm[set_pos]++;
        }
      }  
      if (buttons & BUTTON_DOWN){
        if (setAlarm[set_pos] == 0) {
          setAlarm[set_pos] = 23;
        }
        else {
          setAlarm[set_pos]--;
        }
      }
    }
    else if (set_pos == 1 || set_pos == 2) {
      if (buttons & BUTTON_UP){
        if (setAlarm[set_pos] == 59) {
          setAlarm[set_pos] = 0;
        }
        else {
          setAlarm[set_pos]++;
        }
      }
      if (buttons & BUTTON_DOWN){
        if (setAlarm[set_pos] == 0) {
          setAlarm[set_pos] = 59;
        }
        else {
          setAlarm[set_pos]--;
        }
      }
    }
    else {
      if (buttons & BUTTON_UP){
        Alarm_State = !Alarm_State;
      }
      if (buttons & BUTTON_DOWN){
        Alarm_State = !Alarm_State;
      }    
    }

    //deals with left/right buttons
    //allows user to select between hr/min/sec to be edited
    if (buttons & BUTTON_LEFT) {
      if (set_pos == 0) {
        set_pos = 3;
      }
      else {
        set_pos--;
      }
    }
    if (buttons & BUTTON_RIGHT) {
      if (set_pos == 3) {
        set_pos = 0;
      }
      else {
        set_pos++;
      }
    }

    //change state to CLOCK if BUTTON_SELECT pressed (sets time with values in setValues[])
    if (buttons & BUTTON_SELECT) {
      set_pos = 0;
      lcd.noCursor();
      lcd.clear();

      if (Alarm_State) {
        lcd.setCursor(0,0);
        lcd.print("Alarm set");
        delay(1000);
        lcd.clear();
      }
      state = CLOCK;
    }
  break;

  //current time == set alarm time, snooze/turn off
  case ALARM_TRIGGERED:
    //display
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WAKE UP!   (SEL)");
    lcd.setCursor(0,1);
    lcd.print("(<) Snooze +1min");

    //adds 1min to alarm time
    if (buttons & BUTTON_LEFT) {
      lcd.clear();
      if (setAlarm[1] < 59) {
        setAlarm[1]++;
      }
      else {
        setAlarm[1] = 0;
        if (setAlarm[0] < 23) {
          setAlarm[0]++;
        }
        else {
          setAlarm[0] = 0;
        }
      }
      state = CLOCK;
    }
    
    if (buttons & BUTTON_SELECT) {lcd.clear(); Alarm_State = false; state = CLOCK;}
  break; 
  }
  delay(30);  
}

//prints current time of clock, calls checkDigits to check for leading 0
void Cur_Time() {
  checkDigits(hour());
  lcd.print(":");
  checkDigits(minute()); 
  lcd.print(":");
  checkDigits(second());
}

//checks if value < 10, if true prints a leading 0 and then value
void checkDigits(int digit) {
  if (digit < 10){
    lcd.print("0");
    lcd.print(digit);
  }
  else {
    lcd.print(digit);
  }
}

//called by interupt on timer1 to ckeck whether current time == set alarm time, goes to state = ALARM_TRIGGERED
void checkAlarmTime() {
  if (Alarm_State) {
    if (hour() == setAlarm[0] && minute() == setAlarm[1] && second() == setAlarm[2]) {
      state = ALARM_TRIGGERED;
    }
  }
}


