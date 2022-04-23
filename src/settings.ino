char alarm1Time[8];
char alarm2Time[8];
char alarm3Time[8];
char alarm4Time[8];
char alarm5Time[8];

const int maxAlarmNums = 5;
char *setting_alarms[maxAlarmNums] = {alarm1Time, alarm2Time, alarm3Time, alarm4Time, alarm5Time}; //https://www.javatpoint.com/cpp-array-of-pointers

TimeGetter tg;

// {id:time {time:, music}, id: {time:, music:}}

// music_option -1 means alarm is deleted

int music_options[maxAlarmNums];

// format as 20 alarms

const int BUTTON_PRESS_BUFFER = 250;

int currNumberAlarms;
int modAlarmNumber;

const int PRINT_ALARMS = 0;
const int DISP = 1;
const int ADD = 2;
const int MODIFY = 3;
const int MODIFY_ALARM = 4;

const int ADD_HOUR = 5;
const int ADD_MIN = 6;
const int ADD_MUSIC = 7;

int alarm_state;

int hour;
int mins;
int music_option;
int setting_state;

uint32_t scroll_timer = millis();
const int scroll_threshold = 150;
const float angle_threshold = 0.3;


void setup_settings(){   // this is called when start setup
  currNumberAlarms = 0;
  setting_state = PRINT_ALARMS;
  tft.setTextSize(1.5);
  alarm_state = ADD_HOUR;
}

void handle_settings()
{
  if (setting_state == PRINT_ALARMS)
  {
    display_alarms();
    setting_state = DISP;
  }
  if (setting_state == DISP)
  {
    // ADD SETTINGS - CHANGE BRIGHTNESS
    // button34.read();
    // if (button34.button_pressed && millis() - button34.button_change_time > BUTTON_PRESS_BUFFER)
    // {
    //   button34.button_change_time = millis();

    // }

    // ADD NEW ALARM
    button45.read();
    if (button45.button_pressed && millis() - button45.button_change_time > BUTTON_PRESS_BUFFER && currNumberAlarms < maxAlarmNums)
    {
      button45.button_change_time = millis();
      hour = 1;
      mins = 0;
      music_option = 0;
      setting_state = ADD;
      scroll_timer = millis();
    }

    // MODIFY ALARM
    button38.read();
    if (button38.button_pressed && millis() - button38.button_change_time > BUTTON_PRESS_BUFFER && currNumberAlarms > 0)
    {
      button38.button_change_time = millis();
      modAlarmNumber = 0;
      setting_state = MODIFY;
      alarm_choice_prints();
    }
  }
  if (setting_state == ADD)
  {
    modify_alarm_2(currNumberAlarms);
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
      hour = 1;
      mins = 0;
      music_option = 0;
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
    modify_alarm_2(modAlarmNumber);
  }
}

void alarm_choice_prints(){
  display_alarms();
  tft.println("");
  tft.println("Button39: Change ID");
  tft.println("Button34: Edit Alarm");
  tft.println("Button45: Delete Alarm");
  tft.printf("Alarm Selected: %d", modAlarmNumber + 1);
}

void display_alarms()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,1);
  // Serial.println(currNumberAlarms);
  for (int i = 0; i < currNumberAlarms; i++)
  {
    if (*setting_alarms[i] != '\0'){
      tft.printf("Alarm%d ", i+1);
      tft.printf("Time: %s", setting_alarms[i]);
      tft.printf(" Music:%d", music_options[i]);
      tft.println("");
    }
    // else{
    //   tft.printf("Alarm Number %d:", i+1);
    //   tft.println("OFF");
    // }
  }
  if (currNumberAlarms == 0){
    tft.println("No Alarms!");
  }
  if (setting_state != MODIFY){
    tft.println("");
    if (currNumberAlarms < maxAlarmNums)
    {
      tft.println("Button45: add new Alarm");
    }
    else{
      tft.println("Max Alarms Set!");
    }
    if (currNumberAlarms != 0){
      tft.println("Button38: modify Alarm");
    }
  }
}

void delete_alarm(int alarm_id){
  // delete: move everything back one
  Serial.println(alarm_id);
  for (int i = alarm_id + 1; i < currNumberAlarms; i++)
  {
    sprintf(setting_alarms[i-1], setting_alarms[i]); 
    music_options[i-1] = music_options[i]; 
  }
  currNumberAlarms-=1;
  *setting_alarms[currNumberAlarms] = '\0';
  music_options[currNumberAlarms] = -1;
  // *setting_alarms[alarm_id] = '\0';
  // music_options[alarm_id] = -1;
}

void print_time(int hour, int minute){
  if (hour > 9 and minute > 9){
    tft.printf("%d:%d", hour, minute);
  }
  else if (minute > 9){
    tft.printf("0%d:%d", hour, minute);
  }
  else if (hour > 9){
    tft.printf("%d:0%d", hour, minute);
  }
  else{
    tft.printf("0%d:0%d", hour, minute);
  }
}

void time_to_str(char* time_arr, int hour, int minute){
  if (hour > 9 and minute > 9){
    sprintf(time_arr, "%d:%d", hour, minute);
  }
  else if (minute > 9){
    sprintf(time_arr, "0%d:%d", hour, minute);
  }
  else if (hour > 9){
    sprintf(time_arr, "%d:0%d", hour, minute);
  }
  else{
    sprintf(time_arr, "0%d:0%d", hour, minute);
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

void modify_alarm_2(int alarm_num)
{
  if (alarm_state == ADD_HOUR)
  {
    float x, y;
    get_angle(&x, &y); //get angle values
    int bv = button39.update(); //get button value
    int res = tg.update(y, bv, 23); //input: angle and button, output String to display on 
    if (res != -1)  {
      hour = res;
      alarm_state = ADD_MIN;
    }
  }
  if (alarm_state == ADD_MIN)
  {
    float x, y;
    get_angle(&x, &y); //get angle values
    int bv = button39.update(); //get button value
    int res = tg.update(y, bv, 59); //input: angle and button, output String to display on 
    if (res != -1)  {
      mins = res;
      alarm_state = ADD_MUSIC;
      time_to_str(setting_alarms[alarm_num], hour, mins);
    }
  }
  if (alarm_state == ADD_MUSIC){
    music_options[alarm_num] = 1;

    if (setting_state == ADD){
      currNumberAlarms++;
    }
    setting_state = PRINT_ALARMS;
    alarm_state = ADD_HOUR;
  }
}