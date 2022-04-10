#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <math.h>

MPU6050 imu; //imu object called, appropriately, imu
TFT_eSPI tft = TFT_eSPI(); // Set up the TFT object

const int LOOP_PERIOD = 40;
const int EXCITEMENT = 1000; //how much force to apply to ball
const int TFT_LEFT = 0;
const int TFT_RIGHT = 127;
const int TFT_TOP = 0;
const int TFT_BOTTOM = 159;
const int BALL_RADIUS = 4;

unsigned long primary_timer; //main loop timer

#define BACKGROUND TFT_BLACK
#define BALL_COLOR TFT_WHITE

const uint8_t BUTTON_PIN1 = 45;
const uint8_t BUTTON_PIN2 = 39;

uint8_t ballCountPress;
uint8_t progressPress;
uint8_t ballCountState;
uint8_t progressState;
unsigned long ballCountTimer;
unsigned long progressTimer;

uint8_t numBalls;
uint8_t isGameActive;


struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

class Ball {
    Vec position;
    Vec velocity;
    Vec acceleration;
    TFT_eSPI* local_tft; //tft
    int BALL_CLR;
    int BKGND_CLR;
    float MASS; //for starters
    int RADIUS; //radius of ball
    float K_FRICTION;  //friction coefficient
    float K_SPRING;  //spring coefficient
    float K_REPULSION;
    int LEFT_LIMIT; //left side of screen limit
    int RIGHT_LIMIT; //right side of screen limit
    int TOP_LIMIT; //top of screen limit
    int BOTTOM_LIMIT; //bottom of screen limit
    int DT; //timing for integration
  public:
    Ball(TFT_eSPI* tft_to_use = &tft, int dt = 40, float x_pos = 64, float y_pos = 80, float x_vel = 0, float y_vel = 0, int rad = BALL_RADIUS, float ms = 1,
         int ball_color = TFT_WHITE, int background_color = BACKGROUND,
         int left_lim = TFT_LEFT, int right_lim = TFT_RIGHT, int top_lim = TFT_TOP, int bottom_lim = TFT_BOTTOM) {
      position.x = x_pos; //x position
      position.y = y_pos; //y position
      velocity.x = x_vel; //x velocity
      velocity.y = y_vel; //y velocity
      acceleration.x = 0; //x acceleration
      acceleration.y = 0; //y acceleration
      local_tft = tft_to_use; //our TFT
      BALL_CLR = ball_color; //ball color
      BKGND_CLR = background_color;
      MASS = ms; //for starters
      RADIUS = rad; //radius of ball
      K_FRICTION = 0.15;  //friction coefficient
      K_SPRING = 0.9;  //spring coefficient
      K_REPULSION = 1500;
      LEFT_LIMIT = left_lim + RADIUS; //left side of screen limit
      RIGHT_LIMIT = right_lim - RADIUS; //right side of screen limit
      TOP_LIMIT = top_lim + RADIUS; //top of screen limit
      BOTTOM_LIMIT = bottom_lim - RADIUS; //bottom of screen limit
      DT = dt; //timing for integration
    }
    void repel(Ball* other) {
      float x_distance = getX() - other->getX();
      float y_distance = getY() - other->getY();
      if (x_distance != 0) {
        float x_force = K_REPULSION / x_distance;
        acceleration.x += x_force / MASS;
      }
      if (y_distance != 0) {
        float y_force = K_REPULSION / y_distance;
        acceleration.y += y_force / MASS;
      }
    }
    void step(float x_force = 0, float y_force = 0 ) {
      x_force -= K_FRICTION * velocity.x;
      y_force -= K_FRICTION * velocity.y;
      acceleration.x += x_force / MASS;
      acceleration.y += y_force / MASS;
      //integrate to get velocity from current acceleration
      velocity.x = velocity.x + 0.001*DT*acceleration.x; //integrate, 0.001 is conversion from milliseconds to seconds
      velocity.y = velocity.y + 0.001*DT*acceleration.y; //integrate!!

      local_tft->drawCircle(position.x, position.y, RADIUS, BKGND_CLR);
      moveBall();
      local_tft->drawCircle(position.x, position.y, RADIUS, BALL_CLR);
      acceleration.x = 0;
      acceleration.y = 0;
    }
    void reset(int x = 64, int y = 32) {
      position.x = x;
      position.y = y;
      velocity.x = 0;
      velocity.y = 0;
      acceleration.x = 0;
      acceleration.y = 0;
    }
    int getX() {
      return position.x;
    }
    int getY() {
      return position.y;
    }
  private:
    void moveBall() {
      float new_x = position.x + velocity.x * DT / 1000;
      if (new_x < LEFT_LIMIT){
        velocity.x *= -K_SPRING;
        position.x = LEFT_LIMIT + (K_SPRING) * (LEFT_LIMIT - new_x);
      }
      else if (new_x > RIGHT_LIMIT){
        velocity.x *= -K_SPRING;
        position.x = RIGHT_LIMIT - (K_SPRING) * (new_x - RIGHT_LIMIT);
      }
      else position.x = new_x;
      
      float new_y = position.y + velocity.y * DT / 1000;
      if (new_y < TOP_LIMIT){
        velocity.y *= -K_SPRING;
        position.y = TOP_LIMIT + (K_SPRING) * (TOP_LIMIT - new_y);
      }
      else if (new_y > BOTTOM_LIMIT){
        velocity.y *= -K_SPRING;
        position.y = BOTTOM_LIMIT - (K_SPRING) * (new_y - BOTTOM_LIMIT);
      }
      else position.y = new_y;
    }
};

Ball balls[50];


void setup() {
  Serial.begin(115200); //for debugging if needed.
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(BACKGROUND);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 0, 1);
  tft.printf("Welcome to n-balls!\nHow many balls: %i  ", 0);

  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  randomSeed(analogRead(0));  //initialize random numbers
  delay(20); //wait 20 milliseconds

  ballCountState = 0;
  progressState = 0;
  isGameActive = 0;
  numBalls = 0;

  ballCountTimer = millis();
  progressTimer = millis();
  primary_timer = millis();
}


void loop() {
  ballCountPress = buttonMachine(digitalRead(45), &ballCountState, &ballCountTimer);
  progressPress = buttonMachine(digitalRead(39), &progressState, &progressTimer);  

  switch (isGameActive) {
    case 0:
      if (ballCountPress) {
        numBalls++;
        if (numBalls > 30) numBalls = 0;
        tft.setCursor(0, 0, 1);
        tft.printf("Welcome to n-balls!\nHow many balls: %i  ", numBalls);
      }
      if (progressPress) {
        memset(balls, 0, sizeof(balls));
        for (int i = 0; i < numBalls; i++) {
          int xPosRandom = random(TFT_RIGHT - TFT_LEFT - 2 * BALL_RADIUS) + TFT_LEFT + BALL_RADIUS;
          int yPosRandom = random(TFT_BOTTOM - TFT_TOP - 2 * BALL_RADIUS) + TFT_TOP + BALL_RADIUS;
          int xVelRandom = random(200) - 100;
          int yVelRandom = random(200) - 100;
          balls[i] = Ball(&tft, LOOP_PERIOD, xPosRandom, yPosRandom, xVelRandom, yVelRandom);
        }
        tft.fillScreen(BACKGROUND);
        isGameActive = 1;
      }
      break;
    case 1:
      imu.readAccelData(imu.accelCount); //read imu
      float x = imu.accelCount[0] * imu.aRes;
      float y = imu.accelCount[1] * imu.aRes;
      for (int i = 0; i < numBalls; i++) {
        for (int j = 0; j < numBalls; j++) {
          if (i != j) {
            balls[i].repel(&balls[j]);            
          }
        }
      }
      for (int i = 0; i < numBalls; i++) {
        balls[i].step(y * EXCITEMENT, x * EXCITEMENT);
      }      
      if (progressPress) {
        numBalls = 0;
        tft.fillScreen(BACKGROUND);
        tft.setCursor(0, 0, 1);
        tft.printf("Welcome to n-balls!\nHow many balls: %i  ", numBalls);
        isGameActive = 0;
      }
  }
  while (millis() - primary_timer < LOOP_PERIOD); //wait for primary timer to increment
  primary_timer = millis();
}


uint8_t buttonMachine(uint8_t buttonValue, uint8_t* state, unsigned long* timer) {
  switch (*state) {
    case 0:
      if (!buttonValue) {
        *state = 1;
        *timer = millis();
      }
      break;
    case 1:
      if (buttonValue) {
        *state = 0;        
        if (millis() - *timer > 30) {
          return 1;
        }        
      }
  }
  return 0;
}
