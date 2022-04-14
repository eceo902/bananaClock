#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <mpu6050_esp32.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#define BACKGROUND TFT_BLACK
#define BALL_COLOR TFT_BLUE

const int DT = 10; //milliseconds

struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

uint32_t primary_timer; //main loop timer

//state variables:
struct Vec position; //position of ball
struct Vec velocity; //velocity of ball

const int RADIUS = 3; //radius of ball

//boundary constants:
const int LEFT_LIMIT = RADIUS; //left side of screen limit
const int RIGHT_LIMIT = 127 - RADIUS; //right side of screen limit
const int TOP_LIMIT = RADIUS; //top of screen limit
const int BOTTOM_LIMIT = 159 - RADIUS; //bottom of screen limit

int i;


void step(float upDownVal, float leftRightVal) {
  velocity.x = 0;
  if (upDownVal > 3200) velocity.x = -1;
  else if (upDownVal < 1500) velocity.x = 1;
  velocity.y = 0;
  if (leftRightVal > 3200) velocity.y = -1;
  else if (leftRightVal < 1500) velocity.y = 1;
  moveBall();
}

void moveBall() {
  float new_x = position.x + velocity.x;
  if (new_x < LEFT_LIMIT){
    position.x = LEFT_LIMIT;
  }
  else if (new_x > RIGHT_LIMIT){
    position.x = RIGHT_LIMIT;
  }
  else position.x = new_x;
  
  float new_y = position.y + velocity.y;
  if (new_y < TOP_LIMIT){
    position.y = TOP_LIMIT;
  }
  else if (new_y > BOTTOM_LIMIT){
    position.y = BOTTOM_LIMIT;
  }
  else position.y = new_y;
}




void setup() {
  analogReadResolution(12);       // initialize the analog resolution

  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(BACKGROUND);
  delay(100);
  Serial.begin(115200); //for debugging if needed.

  i = 0;
  tft.setCursor(0, 0, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Q W E R T Y U I O P\n");
  tft.println("A S D F G H J K L\n");
  tft.println("Z X C V B N M");
  
  primary_timer = millis();
}

void loop() {
  int upDown = analogRead(1);
  Serial.println(upDown);
  int leftRight = analogRead(2);
  Serial.println(leftRight);

  
  int prevX = position.x;
  int prevY = position.y;
  step(upDown, leftRight);
  if (position.x != prevX || position.y != prevY) {  
    //draw circle in previous location of ball in color background (redraws minimal num of pixels, therefore is quick!)
    tft.fillCircle(prevX, prevY, RADIUS, BACKGROUND);

    i += 1;
    if (i == 5) {
      tft.setCursor(0, 0, 1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.println("Q W E R T Y U I O P\n");
      tft.println("A S D F G H J K L\n");
      tft.println("Z X C V B N M");
      i = 0;
    }
    tft.fillCircle(position.x, position.y, RADIUS, BALL_COLOR); //draw new ball location
  }

  while (millis() - primary_timer < DT); //wait for primary timer to increment
  primary_timer = millis();
}