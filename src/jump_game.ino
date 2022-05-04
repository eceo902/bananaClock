#include <mpu6050_esp32.h>
#include<math.h>
#include<string.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


uint8_t jump_game_state; //state variable
int jumps; //counting steps
float old_acc_mag, older_acc_mag; //maybe use for remembering older values of acceleration magnitude
float acc_mag = 0;  //used for holding the magnitude of acceleration
float avg_acc_mag = 0; //used for holding the running average of acceleration magnitude

const float ZOOM = 9.81; //for display (converts readings into m/s^2)...used for visualizing only
float x, y, z; //variables for grabbing x,y,and z values
//Serial printing:
char output[100];

const uint8_t LOOP_PERIOD = 10; //milliseconds
//uint32_t jump_timer = 0;

const float threshold = 17;

// jumping states
const uint8_t REST = 0; //example definition
const uint8_t JUMPED = 1; //example...

MPU6050 imu; //imu object called, appropriately, imu

uint8_t isJumpGameActive = 0;

void setup() {
  Serial.begin(115200); //for debugging if needed.
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
  tft.init(); //initialize the screen
  tft.setRotation(2); //set rotation for our layout
  //jump_timer = millis();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  jumps = 0; //initialize steps to zero!
}

// void jump_setup() {
//   tft.init(); //initialize the screen
//   tft.setRotation(2); //set rotation for our layout
//   //jump_timer = millis();
//   tft.fillScreen(TFT_BLACK);
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   jumps = 0; //initialize steps to zero!
// }

void loop() {
  playjumpgame();
}

void playjumpgame(){
  switch (isJumpGameActive){
    case 0:
      averageaccel(); // update average

      countjumps();

      sprintf(output, "# Jumps: %d", jumps); //renders step/jump count formatting
      tft.setCursor(0, 0);
      tft.setTextSize(2); 
      Serial.println(output); //print to serial for plotting

      tft.println(output);
      //tft.println("# Jumps: ");
      //tft.println(steps);
      
      //while (millis() - jump_timer < LOOP_PERIOD); //wait for primary timer to increment
      //jump_timer = millis();
      
      if (jumps == 10){
        isJumpGameActive = 1;
      }
      break;
    case 1:
      tft.setCursor(0, 0);
      tft.setTextSize(2); 
      
      tft.println("Game             \nCompleted");
      Serial.println("Game Completed");


      break;
  }
}

void averageaccel(){
  imu.readAccelData(imu.accelCount);  
  x = ZOOM * imu.accelCount[0] * imu.aRes;
  y = ZOOM * imu.accelCount[1] * imu.aRes;
  z = ZOOM * imu.accelCount[2] * imu.aRes;

  acc_mag = sqrt (x*x+y*y+z*z);

  if (old_acc_mag && older_acc_mag){
    avg_acc_mag = (acc_mag+old_acc_mag+older_acc_mag)/3; //average magnitude
  } else if (old_acc_mag && not older_acc_mag){
    older_acc_mag = old_acc_mag;
  }
  old_acc_mag = acc_mag;

}


void countjumps(){
  switch(jump_game_state){
    case REST:
      if (avg_acc_mag >= threshold){
        jumps ++;
        jump_game_state = JUMPED;
      }
      break;
    case JUMPED:
      if (avg_acc_mag < threshold){
        jump_game_state = REST;
      }
      //while (millis() - jump_timer < 75);
      break;
  }
}

