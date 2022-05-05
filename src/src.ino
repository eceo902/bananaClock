#include <WiFi.h> //Connect to WiFi Network
#include <mpu6050_esp32.h> //Connect to imu
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include "Button.h"
#include "images.h"
//CURRENT BUGS
//I think the timout after 1 min doesn't play the right music
//star wars song is blocking and doesn exit in the middle
//setting a time adds a +1
//the alarm song onlly plays onece, not forever
//if you finish the game before 1 minute, it goes right back to ringing, need to use HasRung

int musicIndex = -1;
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

float x, y, z; //variables for grabbing x,y,and z values

MPU6050 imu; //imu object called, appropriately, imu

bool blocked = false;
unsigned long game_timer;
int mainState = 0;

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



char network[] = "MIT GUEST";
char password[] = "";

Button button45(45);
Button button39(39);
Button button38(38);
Button button34(34);

boolean hasRung;
void playmusic(){
  if (musicIndex == 0){
    pirates();
  } else if (musicIndex == 1){
    harryPotter();
  } else if (musicIndex == 2){
    mario();
  } else if (musicIndex == 4){
    starWars();
  } if (musicIndex == 3){

    pinkPanther();
  }
}

void postWinning(){
  char body[200];
  sprintf(body, "username=%s", letters);
    
}

void setup(){
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

  pinMode(45, INPUT_PULLUP); // first button
  pinMode(39, INPUT_PULLUP); // second button
  pinMode(38, INPUT_PULLUP); // third button
  pinMode(34, INPUT_PULLUP); // fourth button

  pinMode(14, OUTPUT);

  ledcSetup(0, 200, 12);//12 bits of PWM precision
  ledcWrite(0, 0); //0 is a 0% duty cycle for the NFET
  ledcAttachPin(14, 0);

  // For the car motors
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  game_timer=millis();

  setup_clock();
  setup_settings();
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
    const int scroll_threshold = 150;
    const float angle_threshold = 0.3;
  public:

    gameChooser() {
      state = 0;
      game_index = 0;
      scroll_timer = millis();
    }

    void update(float angle, int button, bool alarm) {

      if ((alarm == true) && (state == 0)){
        state = 1;
        Serial.println("Alarm ringing, starting state 1");
        
        //ADD CLOCK BACKGROUND
        tft.setSwapBytes(true); 
        tft.pushImage(0, 0, 640, 480, clockImage);
        tft.setRotation(2);
        tft.setTextSize(1);
        tft.setCursor(10, 40);
        playmusic();

      } else if (state == 1){

        if (button == 1){ //DEACTIVATING THE ALARM
          Serial.println("Pressing button, deactivating alarm, moving to state 2");
          state = 2;
          tft.pushImage(0, 0, 480, 320, test);
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0,  20, 2);
          tft.println("   Math");
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Jumping");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Maze");
          ledcWriteTone(0, 0);

          game_timer = millis();
        }
      } else if (state == 2){ //GAME SELECTION

	      if((button==1) && (millis() - game_timer >= 100)){ //PRESSED BUTTON TO SELECT GAME
		
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          if (game_index == 2){
            tft.println("Playing the Math Game!");
            math_setup();
            state = 3;
          } else if(game_index == 0){
            tft.println("Playing the Jumping Game!");
            state = 4;
            jump_setup();            
          } else {
            tft.println("Playing the Maze Game!");
            state = 5;
          }
          game_index=0;
          scroll_timer=millis();

        } else if (millis() - game_timer >= 60000 ){ //IF THEY TAKE TOO LONG TO DECIDE, ALARM RINGS AGAIN
          state = 1;
          ledcWriteTone(0, 220);
          tft.pushImage(0, 0, 640, 480, clockImage);
  
      } else if (scroll_threshold<=millis()-scroll_timer){
        scroll_timer=millis();
        bool changed = true;
        if (angle_threshold <= angle){

          if(game_index>=2){
            game_index=0;
          }else{
            game_index+=1;	
          }

        } else if(angle<=-angle_threshold){ //CHANGING GAME SELETIONG
          if(game_index<=0){
              game_index=2;
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
      } else if (game_index == 1){

          //tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0, 20, 2);
          tft.println("   Math");
          
          tft.println("   Jumping");
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Maze");
      } else {
          //tft.fillScreen(TFT_BLACK);
          tft.setCursor(10, 40);
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.drawString(" Choose Your Game:", 0, 20, 2);
          tft.setTextColor(TFT_DARKGREY, TFT_SKYBLUE);
          tft.println("   Math");
          tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          tft.println("   Jumping");
          tft.println("   Maze");
      }
    }


    }
  } else if ((state == 5)){ //SEPARATE THIS AS GAMES ARE DONE
    if (millis() - game_timer >= 60000 ){
    state = 1;
    Serial.println("timeout, back to state 1");
    ledcWriteTone(0, 220);
    tft.pushImage(0, 0, 640, 480, clockImage);
    } else if (button == 1){
    state = 0;
    tft.fillScreen(TFT_BLACK);
    tft.println("Good morning! You have completed the game :)");
    mainState = 0;
    Serial.println("finished game, congratulations!");
    delay(5000);
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    }
  } else if (state == 3){
    int mathGameVal = math_loop();
    if (mathGameVal != -1){
      state = 5;
    }
  } else if (state == 4){
    int currentJumpGame = playjumpgame();
    if (currentJumpGame != -1){
      state = 5;
      postWinning();
    }
  }
}
};
gameChooser wg; //wikipedia object



void loop(){
  float x, y;
  get_angle(&x, &y); //get angle values
  int bv = button34.update(); //get button value

  if (mainState == 0){ //MAIN TIME DISPLAYED PAGE
    char* time = loop_clock();
    //if (strcmp(time, "06:48") == 0) {
    musicIndex = activeAlarm1();
    if (musicIndex != -1){
      Serial.println("ALARM RINGING");
      
    mainState = 1;
    wg.update(x, bv, true); //input: angle and button, output String to display on this timestep
    Serial.println("in here");
    } else if (bv == 1){
      mainState = 2;
    }
  } else if (mainState == 1){ //ALARM ACTIVATED
    wg.update(x, bv, false); //input: angle and button, output String to display on this timestep


  } else if (mainState == 2){ //SETTINGS PAGE
    tft.setTextSize(1.5);
    handle_settings();
    if (bv == 1){
      mainState = 0;
      tft.fillScreen(TFT_BLACK);
    }
  }

}   
