#include <mpu6050_esp32.h>
#include<math.h>
#include<string.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


uint8_t state=0; //state variable
int steps = -1; //counting steps
float old_acc_mag, older_acc_mag; //maybe use for remembering older values of acceleration magnitude
float acc_mag = 0;  //used for holding the magnitude of acceleration
float avg_acc_mag = 0; //used for holding the running average of acceleration magnitude

const float ZOOM = 9.81; //for display (converts readings into m/s^2)...used for visualizing only
float x, y, z; //variables for grabbing x,y,and z values
//Serial printing:
char output[100];

const uint8_t LOOP_PERIOD = 10; //milliseconds
uint32_t step_timer = 0;

// jumping states
const uint8_t REST = 0; //example definition
const uint8_t STEPPED = 1; //example...

MPU6050 imu; //imu object called, appropriately, imu

uint8_t isGameActive = 0;

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
  step_timer = millis();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  steps = -1; //initialize steps to zero!
}

void loop() {
  playgame();
}

void playgame(){
  switch (isGameActive){
    case 0:
      averageaccel(); // update average

      countsteps();

      //sprintf(output, "X:%4.2f,Y:%4.2f", acc_mag, avg_acc_mag); //render numbers with %4.2 float formatting
      sprintf(output, "# Jumps: %d", steps); //renders step/jump count formatting
      //Serial.println(steps);
      tft.setCursor(0, 0);
      tft.setTextSize(2); 
      Serial.println(output); //print to serial for plotting

      //tft.println(output);
      tft.println("# Jumps: ");
      tft.println(steps);
      
      while (millis() - step_timer < LOOP_PERIOD); //wait for primary timer to increment
      step_timer = millis();
      
      if (steps == 10){
        isGameActive = 1;
      }
      break;
    case 1:
      tft.setCursor(0, 0);
      tft.setTextSize(2); 
      tft.println("Game Completed                                        ");
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


void countsteps(){
  switch(state){
    case REST:
      if (avg_acc_mag >= 8){
        steps ++;
        state = STEPPED;
      }
      break;
    case STEPPED:
      if (avg_acc_mag <= 6){
        state = REST;
      }
      while (millis() - step_timer < 100);
      break;
  }
}
