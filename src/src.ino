#include <WiFi.h> //Connect to WiFi Network
#include <mpu6050_esp32.h> //Connect to imu
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include "Button.h"
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

WiFiClientSecure client; //global WiFiClient Secure object
WiFiClient client2; //global WiFiClient Secure object
#include "images.h"

int musicIndex = -1;
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

float x, y, z; //variables for grabbing x,y,and z values

MPU6050 imu; //imu object called, appropriately, imu

bool blocked = false;
unsigned long game_timer;
int mainState = 0;

float ambientAmt = 0.0;

char shareData[] = "True";

// brightness variables
const int LCD_PIN = 37;         //pin we use for PWM on LCD
const int pwm_channel = 2; 


//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 2000; //periodicity of getting a number fact.
const int BUTTON_TIMEOUT = 1000; //button timeout in milliseconds
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response
char response[1000];		   // char array buffer to hold HTTP request
char letters[200];  // char array for keyboard
char prompt[200];
char username[200] = "";
float game_time = 23;
char game_name[100] = "math";
char on_leaderboard[10] = "True"; // TODO: set based on log-in by Tues 

int masterState;
const int IN_CLOCK = 0;
const int IN_SETTINGS = 2;
bool loggedIn;

char network[] = "MIT";
char password[] = "";

Button button45(45);
Button button45Testing(45);
Button button39(39);
Button button39Clock(39);
Button button38(38);
Button button34(34);
Button button34Testing(34);
Button button34Clock(34);
Button button34Settings(34);

int bv34; //get button value
int bv39; //get button value
int bv38; //get button value
int bv45;

bool hasRung;
void playmusic(){
  if (musicIndex == 0){
    throne();
  } else if (musicIndex == 1){
    harryPotterLoop();
  } else if (musicIndex == 2){
    mario();
  } else if (musicIndex == 4){
    starWars();
  } if (musicIndex == 3){

    pinkPanther();
  }
}

unsigned long musicTiming = millis();
int currNoteIndex = 0;

void postWinning(){
  char body[200];
  sprintf(body, "user=%s&game_name=%s&length=%f&on_leaderboard=%s", username, game_name, game_time, on_leaderboard);
	sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team41/game_data/save_game_results.py HTTP/1.1\r\n");
	strcat(request_buffer, "Host: 608dev-2.net\r\n");
	strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
	sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
	strcat(request_buffer, "\r\n"); //new line from header to body
	strcat(request_buffer, body); //body
	strcat(request_buffer, "\r\n"); //new line
	Serial.println(request_buffer);
	do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
	Serial.println(response_buffer); //viewable in Serial Terminal
	//memset(letters, 0, sizeof(letters));
    
}

void setup(){
  analogReadResolution(12);

  tft.init();  //init screen
  tft.setRotation(1); //adjust rotation
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  Serial.begin(115200); //begin serial comms
  delay(50); //pause to make sure comms get set up
  Wire.begin();
  delay(50); //pause to make sure comms get set up
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count<6) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n",WiFi.localIP()[3],WiFi.localIP()[2],
                                            WiFi.localIP()[1],WiFi.localIP()[0], 
                                          WiFi.macAddress().c_str() ,WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  // For the buttons
  pinMode(45, INPUT_PULLUP); // first button
  pinMode(39, INPUT_PULLUP); // second button
  pinMode(38, INPUT_PULLUP); // third button
  pinMode(34, INPUT_PULLUP); // fourth button


  pinMode(14, OUTPUT);

  // ledcSetup(0, 200, 12);//12 bits of PWM precision
  ledcSetup(pwm_channel, 200, 12);//12 bits of PWM precision
  // ledcWrite(0, 0); //0 is a 0% duty cycle for the NFET
  ledcAttachPin(14, 0);


    //Turn on LCD_PIN as output (for driving transistor)
  ledcAttachPin(LCD_PIN, pwm_channel);
  pinMode(LCD_PIN, OUTPUT); 


  // For the horn
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);


  // For the car motors
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  game_timer=millis();

  hasRung = false;
  loggedIn = false; // user has not logged in when program runs
  setup_login();
}


//used to get x,y values from IMU accelerometer!
void get_angle(float* x, float* y) {
  imu.readAccelData(imu.accelCount);
  *x = imu.accelCount[0] * imu.aRes;
  *y = imu.accelCount[1] * imu.aRes;
}



class gameChooser {
    int game_index;
    int state;
    uint32_t scroll_timer;
    const int scroll_threshold = 500;
    const float angle_threshold = 0.3;
  public:

    gameChooser() {
      state = 0;
      game_index = 0;
      scroll_timer = millis();
    }
    void alarmRinging(){
      digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
        setup_car();
        state = 1;
        Serial.println("Alarm ringing, starting state 1");
        
        //ADD CLOCK BACKGROUND
       // tft.setSwapBytes(true); 
        //tft.pushImage(0, 0, 640, 480, clockImage);
  tft.fillScreen(TFT_BLACK);
    tft.setRotation(2);
        tft.setTextSize(1);
        tft.setCursor(10, 40);
        tft.println();
        tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.drawString(" Alarm Ringing", 0, 20, 2);
       
      
      
        musicTiming = millis();
        currNoteIndex = 0;
        playmusic();
    }    

    void update(float angle, int button, bool alarm) {

      if ((alarm == true) && (state == 0)){
        alarmRinging();

      } else if (state == 1){

        if (button == 1){ //DEACTIVATING THE ALARM
          stop_car();
          Serial.println("Pressing button, deactivating alarm, moving to state 2");
          state = 2;
          tft.fillScreen(TFT_BLACK);
          //tft.pushImage(0, 0, 480, 320, test);
          
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0,  20, 2);
          tft.println("   Math");
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Jumping");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Maze");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Cipher");
          ledcWriteTone(0, 0);

          game_timer = millis();
        } else {
          loop_car();
          playmusic();
        }
      } else if (state == 2){ //GAME SELECTION

	      if((button==1) && (millis() - game_timer >= 100)){ //PRESSED BUTTON TO SELECT GAME
		
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          game_timer = millis();
          if (game_index == 3){
            tft.println("Playing the Math Game!");
            math_setup();
            state = 3;
        
          } else if(game_index == 0){
            tft.println("Playing the Jumping Game!");
            state = 4;
            jump_setup();      
            sprintf(game_name, "jumping");      
          } else if (game_index == 1){
            tft.println("Playing the Maze Game!");
            state = 7;
            setupMaze();
            sprintf(game_name, "maze");
          } else {
            tft.println("Playing Cipher!");
            cipher_setup();
            state = 6;
          }
          game_index=0;
          scroll_timer=millis();

        } else if (millis() - game_timer >= 60000 ){ //IF THEY TAKE TOO LONG TO DECIDE, ALARM RINGS AGAIN
          state = 1;
          alarmRinging();
          //tft.pushImage(0, 0, 640, 480, clockImage);
          
  
      } else if (scroll_threshold<=millis()-scroll_timer){
        scroll_timer=millis();
        bool changed = true;
        if (angle_threshold <= angle){

          if(game_index>=3){
            game_index=0;
          }else{
            game_index+=1;	
          }

        } else if(angle<=-angle_threshold){ //CHANGING GAME SELETIONG
          if(game_index<=0){
              game_index=3;
          }else{
              game_index-=1;
          }   
        } else {
          changed = false;
        }

    if (changed == true){ //CHANGED GAME SELECTION
      if (game_index == 0){
          //tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0,  20, 2);
          tft.println("   Math");
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Jumping");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Maze");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Cipher");
      } else if (game_index == 1){

          //tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0, 20, 2);
          tft.println("   Math");
          
          tft.println("   Jumping");
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Maze");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Cipher");
      } else if (game_index == 3){
          //tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0, 20, 2);
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Math");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Jumping");
          tft.println("   Maze");
          tft.println("   Cipher");
      } else {
        tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0, 20, 2);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Math");
          
          tft.println("   Jumping");
          tft.println("   Maze");
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Cipher");
      }
    }


    }
  } else if ((state == 5)){ //GAME WON!
    hasRung = true;
    state = 0;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("Good morning!");
    tft.println();
    tft.println(" You have completed");
    tft.println(" the game :)");
    mainState = 1;
    Serial.println("finished game, congratulations!");
    delay(5000);
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    game_time = (millis() - game_timer)/1000;
    Serial.println(game_time);
    postWinning();
    setup_clock();
    
    //}
  } else if (state == 3){
    int mathGameVal = math_loop();
    Serial.println(mathGameVal);
    Serial.println("printing");
    if (mathGameVal != -1){
      state = 5;
    }
    if (millis() - game_timer >= 60000 ){ //IF THEY TAKE TOO LONG TO DECIDE, ALARM RINGS AGAIN
          state = 1;
          alarmRinging();
    }
  } else if (state == 7){
    int mazevar = loopMaze();
    if (mazevar != -1){
      state = 5;
    }
    if (millis() - game_timer >= 60000 ){ //IF THEY TAKE TOO LONG TO DECIDE, ALARM RINGS AGAIN
          state = 1;
          alarmRinging();
    }
  
  }else if (state == 6){
    int cipherGameVal = cipher_loop();
    // Serial.println(cipherGameVal);
    // Serial.println("printing");
    if (cipherGameVal != -1){
      state = 5;
    }
    if (millis() - game_timer >= 600000 ){ //IF THEY TAKE TOO LONG TO DECIDE, ALARM RINGS AGAIN
          state = 1;
          alarmRinging();
    }
  } else if (state == 4){
    int currentJumpGame = playjumpgame();
    if (currentJumpGame != -1){
      //postWinning();
      state = 5;
      
    }
    if (millis() - game_timer >= 60000 ){ //IF THEY TAKE TOO LONG TO DECIDE, ALARM RINGS AGAIN
          state = 1;
          alarmRinging();
    }
  }
}
};
gameChooser wg; //wikipedia object



void loop(){
  ledcWrite(pwm_channel, ambientAmt);

  get_angle(&x, &y); //get angle values

  // button39.read(); //get button value
  bv34 = button34.update(); //get button value
  bv39 = button39.update(); //get button value
  bv38 = button38.update(); //get button value
  bv45 = button45.update();
  // int b39C = button39Clock.update();

  if (mainState == 0){
    

    int loopTemp = loop_login();
    if (loopTemp != -1) {
      mainState = 1;
      loggedIn = true;
      setup_clock();
      get_alarms_user(); // pull users' alarms at beginning of program, MUST run after username set
      sprintf(on_leaderboard, "%s", "True"); // reset to True on each login
     }


  } else if (mainState == 1){ //MAIN TIME DISPLAYED PAGE
    char* time = loop_clock(bv45, bv39, bv38);
    //if (strcmp(time, "06:48") == 0) {
    musicIndex = activeAlarm1(time);

    //DELETE SECOND PART OF IF
    if ((musicIndex != -1 ) && (hasRung == false)){
      Serial.println("ALARM RINGING");
      tft.fillScreen(TFT_BLACK);
      //tft.println("Alarm Ringing");
      setup_car();
      
    mainState = 2;
    wg.update(x, bv34, true); //input: angle and button, output String to display on this timestep
    } else if ((musicIndex != -1 )){
      hasRung = false;
    }
    if (bv34 != 0){ // USER SETTINGS

      mainState = 4; 
    }
  } 
  else if (mainState == 2){ //ALARM ACTIVATED
    wg.update(x, bv34, false); //input: angle and button, output String to display on this timestep


  } else if (mainState == 5){ //SETTINGS PAGE
    tft.setTextSize(1.5);
    loggedIn = true;
    int result = handle_settings(bv34, bv39, bv38, bv45);
    if (result == 1) {
      mainState = 1;
      setup_clock();
    }
    else if (result == 2){
      loggedIn = false;
      mainState = 0;
      setup_login();
    }
  }
  else if (mainState == 4){    // go into settings
    int result = handle_user_settings(bv39, bv38);

    if (result == 1) // go to alarm settings
    {
      goto_settings();
      // if (!loggedIn){ 
      //   get_alarms_user(); // pull users' alarms from db
      //   loggedIn = true;
      // }
      mainState = 5;
      // setup_login();    
    }
  }
  }  
