// TODO: add something to change if data shared
// TODO: fix time changing

const uint16_t JSON_BODY_SIZE = 4000;

const int BUFFER_SIZE_SETTINGS = 10000;

char request_get[BUFFER_SIZE_SETTINGS]; //char array buffer to hold HTTP request
char response_get[BUFFER_SIZE_SETTINGS]; //char array buffer to hold HTTP response

char request_post[BUFFER_SIZE_SETTINGS]; //char array buffer to hold HTTP request
char response_post[BUFFER_SIZE_SETTINGS]; //char array buffer to hold HTTP response
char json_body[BUFFER_SIZE_SETTINGS];

int offset;

char alarm1Time[8];
char alarm2Time[8];
char alarm3Time[8];
char alarm4Time[8];
char alarm5Time[8];

const int maxAlarmNums = 5;
char *setting_alarms[maxAlarmNums] = {alarm1Time, alarm2Time, alarm3Time, alarm4Time, alarm5Time}; //https://www.javatpoint.com/cpp-array-of-pointers

// user username[] array instead of this

TimeGetter tg;

// send kristine a list of alarms in the POST request as well as music options as a list up to all the valid alarms
// 1 param in GET request & that's the username, return as JSON
// lists returned have parallel structure as in POST request
// {id:time {time:, music}, id: {time:, music:}}
int musicState = 0;

// music_option -1 means alarm is deleted

int music_options[maxAlarmNums];


// format as 20 alarms

const int BUTTON_PRESS_BUFFER = 300;

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

int button39ChangeTime = 0;

uint32_t scroll_timer = millis();
const int scroll_threshold = 150;
const float angle_threshold = 0.3;

void goto_settings(){
  setting_state = PRINT_ALARMS;
  tft.setTextSize(1.5);
  alarm_state = ADD_HOUR;
}

void get_alarms_db(){  

  Serial.println("GETTING");
  
  request_get[0] = '\0'; //set 0th byte to null
  memset(response_get, 0, sizeof(response_get)); //set 0th byte to null

  sprintf(request_get, "GET  http://608dev-2.net/sandbox/sc/team41/alarms/get_alarms.py?user=%s HTTP/1.1\r\n", username);
  strcat(request_get, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_get, response_get, BUFFER_SIZE_SETTINGS, RESPONSE_TIMEOUT, true);

  DynamicJsonDocument doc1(500);
  DeserializationError error = deserializeJson(doc1, response_get);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
  }

  music_options[0] = {doc1["doc"]["music"]["m0"]};
  music_options[1] = {doc1["doc"]["music"]["m1"]};
  music_options[2] = {doc1["doc"]["music"]["m2"]};
  music_options[3] = {doc1["doc"]["music"]["m3"]};
  music_options[4] = {doc1["doc"]["music"]["m4"]};

  const char* alarmResult1 = doc1["doc"]["alarm_time"]["a0"];
  if (music_options[0] != -1){
    sprintf(setting_alarms[0], alarmResult1);
    currNumberAlarms++;
  }
  else{
    *setting_alarms[0] = '\0';
  }
  
  const char* alarmResult2 = doc1["doc"]["alarm_time"]["a1"];

  if (music_options[1] != -1){
    sprintf(setting_alarms[1], alarmResult2);
    currNumberAlarms++;
  }
  else{
    *setting_alarms[1] = '\0';
  }

  const char* alarmResult3 = doc1["doc"]["alarm_time"]["a2"];

  if (music_options[2] != -1){
    sprintf(setting_alarms[2], alarmResult3);
    currNumberAlarms++;
  }  
  else{
    *setting_alarms[2] = '\0';
  }

  const char* alarmResult4 = doc1["doc"]["alarm_time"]["a3"];

  if (music_options[3] != -1){
    // Serial.println("REACHED_4");
    sprintf(setting_alarms[3], alarmResult4);
    currNumberAlarms++;
  }
  else{
    *setting_alarms[3] = '\0';
  }

  const char* alarmResult5 = doc1["doc"]["alarm_time"]["a4"];

  if (music_options[4] != -1){
    sprintf(setting_alarms[4], alarmResult5);
    currNumberAlarms++;
  }
  else{
    *setting_alarms[4] = '\0';
  }

}

void get_alarms_user()
{   // change to run after user logs in
  currNumberAlarms = 0;
  get_alarms_db();
  // pull current alarms for user and update currNumberAlarms based on that
}

int update_db_alarms(){  

  // Serial.println("POSTING");

  tft.println("Saving your alarms ... hold tight!");

  if (currNumberAlarms < 5){
    for (int i = currNumberAlarms; i < 5; i++){
      sprintf(setting_alarms[i], ".");
    }
  }

// https://arduino.stackexchange.com/questions/84183/how-to-send-int-array-with-esp8266-http-post-request

  char alarmTimesArray[100];
  // sprintf(alarmTimesArray, "[\"%s\",\"%s\",\"%s\"]", setting_alarms[0], setting_alarms[1], setting_alarms[2]);
  sprintf(alarmTimesArray, "[\"%s\", \"%s\", \"%s\", \"%s\", \"%s\"]", setting_alarms[0], setting_alarms[1], setting_alarms[2], setting_alarms[3], setting_alarms[4]);

  char musicOptionsArray[100];
  // sprintf(musicOptionsArray, "[%d,%d,%d]", music_options[0], music_options[1], music_options[2]);
  sprintf(musicOptionsArray, "[%d, %d, %d, %d, %d]", music_options[0], music_options[1], music_options[2], music_options[3], music_options[4]);

  offset = 0;
  offset += sprintf(json_body, "{\"user\"=\"%s\"&", username);
  offset += sprintf(json_body + offset, "\"alarm_time\"=%s&", alarmTimesArray);
  offset += sprintf(json_body + offset, "\"music\"=%s}", musicOptionsArray);
  
  Serial.println("JSON");
  Serial.println(json_body);

  offset = 0;
  int len = strlen(json_body);

  request_post[0] = '\0'; //set 0th byte to null 
  offset += sprintf(request_post + offset, "POST http://608dev-2.net/sandbox/sc/team41/alarms/save_alarms.py?user=%s&alarm_time=%s&music=%s HTTP/1.1\r\n", username, alarmTimesArray, musicOptionsArray);
  offset += sprintf(request_post + offset, "Host: 608dev-2.net\r\n");
  offset += sprintf(request_post + offset, "Content-Type: application/json\r\n");
  offset += sprintf(request_post + offset, "cache-control: no-cache\r\n");
  offset += sprintf(request_post + offset, "Content-Length: %d\r\n\r\n", len);
  offset += sprintf(request_post + offset, "%s\r\n", json_body);
  do_http_request("608dev-2.net", request_post, response_post, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(request_post); 

  return 2; 
}

int handle_settings(int update_34, int update_39, int update_38, int update_45)
{
  tft.setTextSize(1.5);
  if (setting_state == PRINT_ALARMS)
  {
    display_alarms();
    setting_state = DISP;
  }
  if (setting_state == DISP)
  {
    if (update_34 != 0) { // exit settings
        return 1;
      }

    if (update_39 != 0 && millis() - button39ChangeTime >= BUTTON_PRESS_BUFFER) { // LOGOUT of program, post alarms to database            
        return update_db_alarms();
    }

    // ADD NEW ALARM
    if (update_45!= 0 && currNumberAlarms < maxAlarmNums)
    {
      hour = 1;
      mins = 0;
      music_option = 0;
      setting_state = ADD;
      tg.scroll_prints(hour, 23);
      scroll_timer = millis();
    }

    // MODIFY ALARM
    if (update_38 !=0 && currNumberAlarms > 0)
    {
      modAlarmNumber = 0;
      setting_state = MODIFY;
      alarm_choice_prints();
    }
  }
  if (setting_state == ADD)
  {
    modify_alarm_2(currNumberAlarms);
  }
  if (setting_state == MODIFY)
  {
    if (update_39 !=0)
    {
      modAlarmNumber+=1;
      if (modAlarmNumber == currNumberAlarms){
        modAlarmNumber = 0;        
      }
      alarm_choice_prints();
    }
    if (update_34 !=0)
    {
      hour = 1;
      mins = 0;
      music_option = 0;
      tg.scroll_prints(hour, 23);
      setting_state = MODIFY_ALARM;
      scroll_timer = millis();
    }
    if (update_45 != 0)
    {
      delete_alarm(modAlarmNumber);
      setting_state = PRINT_ALARMS;
    }
  }
  if (setting_state == MODIFY_ALARM){
    modify_alarm_2(modAlarmNumber);
  }
  return 0;
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
    tft.println("Button34: Back to Clock");
    if (currNumberAlarms != 0){
      tft.println("Button38: modify Alarm");
    }
    tft.println("Button39: Logout");
  }
}

void delete_alarm(int alarm_id){
  // delete: move everything back one
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
      if (res == 0){
        res = 23;        
      }
      else{
        res -= 1;
      }
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
      if (res == 0){
        res = 60;        
      }
      else{
        res -= 1;
      }
      mins = res;
      alarm_state = ADD_MUSIC;

      tft.fillScreen(TFT_BLACK);
      tft.setCursor(10, 40);
      tft.println("Press 45 to confirm song and 39 to switch song");
      tft.println("");
        tft.println("Playing Game of Thrones");
        throne();

      time_to_str(setting_alarms[alarm_num], hour, mins);
      
    }
  }
  if (alarm_state == ADD_MUSIC){  
  
    if (!digitalRead(BUTTON1)) { //At each Changing State
      musicState += 1;
      //DELETE THIS
      Serial.println("state is changing");
      musicTiming = millis();
      currNoteIndex = 0;

      tft.fillScreen(TFT_BLACK);
      tft.setCursor(10, 40);
      tft.println("Press 45 to confirm song and 39 to switch song");
      tft.println("");

      if (musicState == 0){
        tft.println("Playing Game of Thrones");
        throne();
  } else if (musicState == 1){
        tft.println("Playing Harry Potter Theme Song");
        harryPotterLoop();
  } else if (musicState == 2){
        tft.println("Playing Mario Theme Song");
        mario();
  } else if (musicState == 4){
        tft.println("Playing Star Wars");
        starWars();
  } if (musicState == 3){
    tft.println("Playing Pink Panther");
    pinkPanther();
  } else if (musicState == 5){
      musicState = 0;
    }
    delay(500);
  }

  

  if (musicState == 0){
    throne();
  } else if (musicState == 1){
    harryPotterLoop();
  } else if (musicState == 2){
    mario();
  } else if (musicState == 4){
    starWars();
  } if (musicState == 3){
    pinkPanther();
  }
    if (!digitalRead(BUTTON2)) {
      Serial.println("problem if in here");
    music_options[alarm_num] = musicState;
    ledcWriteTone(AUDIO_PWM, 0);
    if (setting_state == ADD){
      currNumberAlarms++;
    }
    button39ChangeTime = millis();
    setting_state = PRINT_ALARMS;
    alarm_state = ADD_HOUR;
    musicState = 0;
  }

 }   
}

int activeAlarm1(char* time){
  // char* time = "10:00";
  // char* time = loop_clock(update_45, update_39, update_38);
  for (int i = 0; i < currNumberAlarms; i++){
    if (music_options[i] != -1){ // if (*setting_alarms[i] != '\0'){
      if (strcmp(time, setting_alarms[i]) == 0){
        Serial.println("should be returning a valid music option");
        return music_options[i];
      }

    }
  }
  return -1;
}
