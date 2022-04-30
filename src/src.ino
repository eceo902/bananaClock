#include <WiFi.h> //Connect to WiFi Network
#include <mpu6050_esp32.h> //Connect to imu
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include "Button.h"


TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

float x, y, z; //variables for grabbing x,y,and z values
int acc_mag;
MPU6050 imu; //imu object called, appropriately, imu


//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 2000; //periodicity of getting a number fact.
const int BUTTON_TIMEOUT = 1000; //button timeout in milliseconds
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char prompt[100];   // char array for asking user for input
char letters[100];  // char array for keyboard

int masterState;


char network[] = "MIT";
char password[] = "";

Button button45(45);
Button button39(39);
Button button38(38);
Button button34(34);

boolean hasRung;


void setup(){
  analogReadResolution(12);       // initialize the analog resolution

  tft.init();

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

  masterState = 0;

  setup_clock();

  ledcWriteTone(0, 220);
  hasRung = false;
}

void loop(){
  switch(masterState) {
    case 0: {
      char* time = loop_clock();
      if (strcmp(time, "20:53") == 0 && !hasRung) {
        ledcWriteTone(0, 220);
        hasRung = true;
      }
      if (hasRung && button39.update() != 0) {
        ledcWriteTone(0, 0);
      }

      if (button34.update() != 0) {
        masterState = 1;
        strcpy(prompt, "Hi, type to login");
        setup_joystick();
      }
      break;
    }
    case 1: {
      bool hasSubmitted = loop_joystick();
      if (hasSubmitted) {
        char body[100]; //for body
        sprintf(body, "username=%s", letters);
        sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team41/login/esp_login.py HTTP/1.1\r\n");
        strcat(request_buffer, "Host: 608dev-2.net\r\n");
        strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
        sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
        strcat(request_buffer, "\r\n"); //new line from header to body
        strcat(request_buffer, body); //body
        strcat(request_buffer, "\r\n"); //new line
        Serial.println(request_buffer);
        do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
        Serial.println(response_buffer); //viewable in Serial Terminal
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 1);
        tft.println(response_buffer);
        memset(letters, 0, sizeof(letters));
      }
      if (button34.update() != 0) {
        masterState = 0;
        setup_clock();
      }
    }
  }
}   
