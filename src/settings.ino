int alarm_hours[50];
int alarm_minutes[50];
int music_options[50];

const int BUTTON_PRESS_BUFFER = 250;

int currNumberAlarms;

int modAlarmNumber;

const int PRINT_ALARMS = 0;
const int DISP = 1;
const int ADD = 2;
const int MODIFY = 3;
const int MODIFY_ALARM = 4;


int hour;
int mins;
int music_option;


int setting_state;

void setup_settings(){   // this is called when start setup
  currNumberAlarms = 0;
  setting_state = PRINT_ALARMS;
}

void handle_settings(){
  // add an alarm if button 1 pushed
  if (setting_state == PRINT_ALARMS)
  {
    display_alarms();
    setting_state = DISP;
  }
  if (setting_state == DISP)
  {
    button45.read();
    if (button45.button_pressed && millis() - button45.button_change_time > BUTTON_PRESS_BUFFER)
    {
      button45.button_change_time = millis();
      hour = 1;
      mins = 0;
      music_option = 0;
      setting_state = ADD;
      modify_prints(hour, mins, music_option);
    }
    button38.read();
    if (button38.button_pressed && millis() - button38.button_change_time > BUTTON_PRESS_BUFFER && currNumberAlarms > 0)
    {
      button38.button_change_time = millis();
      modAlarmNumber = 0;
      setting_state = MODIFY;
      alarm_choice_prints();
    }

  }
  if (setting_state == ADD){
    modify_alarm(currNumberAlarms);
  }
  if (setting_state == MODIFY){
    button39.read();
    if (button39.button_pressed && millis() - button39.button_change_time > BUTTON_PRESS_BUFFER)
    {
      button39.button_change_time = millis();
      modAlarmNumber+=1;
      if (modAlarmNumber == currNumberAlarms){
        modAlarmNumber = 0;        
      }
      alarm_choice_prints();
    }
    button34.read();
    if (button34.button_pressed && millis() - button38.button_change_time > BUTTON_PRESS_BUFFER)
    {
      button34.button_change_time = millis();
      if (alarm_hours[modAlarmNumber] == -1){
        hour = 1;
        mins = 0;
        music_option = 0;
      }
      else{
        hour = alarm_hours[modAlarmNumber];
        mins = alarm_minutes[modAlarmNumber];
        music_option = music_options[modAlarmNumber];
      }
      setting_state = MODIFY_ALARM;
      modify_prints(hour, mins, music_option);
    }
    button45.read();
    if (button45.button_pressed && millis() - button45.button_change_time > BUTTON_PRESS_BUFFER)
    {
      button45.button_change_time = millis();
      delete_alarm(modAlarmNumber);
      setting_state = PRINT_ALARMS;
    }

  }
  if (setting_state == MODIFY_ALARM){
    modify_alarm(modAlarmNumber);
  }
}

void alarm_choice_prints(){
  display_alarms();
  tft.println("Button39: Change ID");
  tft.println("Button34: Edit Alarm");
  tft.println("Button45: Delete Alarm");
  tft.printf("Alarm Selected: %d", modAlarmNumber + 1);
}

void display_alarms()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,1);
  Serial.println(currNumberAlarms);
  for (int i = 0; i < currNumberAlarms; i++)
  {
    if (alarm_hours[i] != -1){
      tft.printf("Alarm%d ", i+1);
      print_time(alarm_hours[i], alarm_minutes[i]);
      tft.printf(" Music:%d", music_options[i]);
      tft.println("");
    }
    else{
      tft.printf("Alarm Number %d:", i+1);
      tft.println("OFF");
    }
  }
  if (currNumberAlarms == 0){
    tft.println("No Alarms!");
  }
  if (setting_state != MODIFY){
    tft.println("Button45: add new Alarm");
    if (currNumberAlarms != 0){
      tft.println("Button38: modify Alarm");
    }
  }
}

void delete_alarm(int alarm_id){
  alarm_hours[alarm_id] = -1;
  alarm_minutes[alarm_id] = -1;
  music_options[alarm_id] = -1;
}

void print_time(int hour, int minute){
  if (hour > 9 and minute > 9){
    tft.printf("Time:%d:%d", hour, minute);
  }
  else if (minute > 9){
    tft.printf("Time:0%d:%d", hour, minute);
  }
  else if (hour > 9){
    tft.printf("Time:%d:0%d", hour, minute);
  }
  else{
    tft.printf("Time:0%d:0%d", hour, minute);
  }
   
}

void modify_prints(int hour, int min, int music_option){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,1);
  tft.println("Button45: Change Hour");
  tft.println("Button39: Change Min");
  tft.println("Button38: Change Music");
  tft.println("Button34: Confirm");
  tft.println("");
  print_time(hour, min);
  tft.println("");
  tft.printf("Music Selection: %d", music_option);
  tft.println("");
}

void modify_alarm(int alarm_num)
{  
  // char time_alarm[50];
  
  button45.read();
  if (button45.button_pressed && millis() - button45.button_change_time > BUTTON_PRESS_BUFFER)
  {
    button45.button_change_time = millis();
    Serial.printf("Hour %d", hour);
    Serial.println("");
    hour+=1;
    if (hour == 24){
      hour = 0;
    }
    modify_prints(hour, mins, music_option);
  }

  button39.read();
  if (button39.button_pressed && millis() - button39.button_change_time > BUTTON_PRESS_BUFFER)
  {
    button39.button_change_time = millis();
    Serial.printf("Mins %d", mins);
    Serial.println("");
     mins+=1;
    if (mins == 60){
      mins = 0;
    }
    modify_prints(hour, mins, music_option);
  }

  button38.read();
  if (button38.button_pressed && millis() - button38.button_change_time > BUTTON_PRESS_BUFFER)
  {
    button38.button_change_time = millis();
    button38.update();
    Serial.println("");
    music_option+=1;
    modify_prints(hour, mins, music_option);
  }

  button34.read();
  if (button34.button_pressed && millis() - button34.button_change_time > BUTTON_PRESS_BUFFER)
  {
    button34.button_change_time = millis();
    alarm_hours[alarm_num] = hour;
    alarm_minutes[alarm_num] = mins;
    music_options[alarm_num] = music_option;
    Serial.printf("Minutes Set %d", mins);
    Serial.printf("Min Set %d", alarm_minutes[alarm_num]);
    Serial.printf("Music Set %d", music_options[alarm_num]);
    if (setting_state == ADD){
      currNumberAlarms++;
    }
    setting_state = PRINT_ALARMS;
  }
}
