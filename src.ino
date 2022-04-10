#include <WiFi.h> //Connect to WiFi Network
#include <mpu6050_esp32.h> //Connect to imu
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.

const uint8_t OFF = 0;
const uint8_t ON = 1;
const uint8_t ALWAYS_ON = 2;

const uint8_t HOUR_MINUTE = 0;
const uint8_t HOUR_MINUTE_SECOND = 1;

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

char hm[8];
char hms[11];


char network[] = "MIT";
char password[] = "";

uint8_t scanning = 0;//set to 1 if you'd like to scan for wifi networks (see below):
/* Having network issues since there are 50 MIT and MIT_GUEST networks?. Do the following:
    When the access points are printed out at the start, find a particularly strong one that you're targeting.
    Let's say it is an MIT one and it has the following entry:
   . 4: MIT, Ch:1 (-51dBm)  4:95:E6:AE:DB:41
   Do the following...set the variable channel below to be the channel shown (1 in this example)
   and then copy the MAC address into the byte array below like shown.  Note the values are rendered in hexadecimal
   That is specified by putting a leading 0x in front of the number. We need to specify six pairs of hex values so:
   a 4 turns into a 0x04 (put a leading 0 if only one printed)
   a 95 becomes a 0x95, etc...
   see starting values below that match the example above. Change for your use:
   Finally where you connect to the network, comment out 
     WiFi.begin(network, password);
   and uncomment out:
     WiFi.begin(network, password, channel, bssid);
   This will allow you target a specific router rather than a random one!
*/
uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.

uint8_t power_state;
uint8_t style_state;
unsigned long query_timer;
unsigned long power_timer;
unsigned long blink_timer;

uint8_t style_input;
uint8_t style_previous;
uint8_t power_input;
uint8_t power_previous;

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
  
  if (scanning){
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
      Serial.println("no networks found");
    } else {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) {
        Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
        uint8_t* cc = WiFi.BSSID(i);
        for (int k = 0; k < 6; k++) {
          Serial.print(*cc, HEX);
          if (k != 5) Serial.print(":");
          cc++;
        }
        Serial.println("");
      }
    }
  }
  delay(100); //wait a bit (100 ms)

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

  pinMode(45, INPUT_PULLUP); //set input pin as an input!
  pinMode(39, INPUT_PULLUP);

  power_state = ALWAYS_ON;
  style_state = HOUR_MINUTE;

  power_previous = 1;
  style_previous = 1;

  sprintf(request_buffer, "GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
  strcat(request_buffer, "Host: iesc-s3.mit.edu\r\n"); //add more to the end
  strcat(request_buffer, "\r\n"); //add blank line!
  //submit to function that performs GET.  It will return output using response_buffer char array
  do_http_GET("iesc-s3.mit.edu", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  strncpy(hm, &response_buffer[11], 5); 
  strncpy(hms, &response_buffer[11], 8);
  if (get_time(hm, 0) > 12){
    hm[5] = 'p';
    hm[6] = 'm';
    hms[8] = 'p';
    hms[9] = 'm';
  }
  else {
    hm[5] = 'a';
    hm[6] = 'm';
    hms[8] = 'a';
    hms[9] = 'm';
  }
  normalize_hour(hm); 
  normalize_hour(hms);
  Serial.println(response_buffer);
  Serial.println(hm);
  Serial.println(hms);

  query_timer = millis();
  blink_timer = millis(); // we also need to reset the blink timer so no drifting
}

void loop(){
  uint8_t style_input = digitalRead(45);
  uint8_t power_input = digitalRead(39);
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  z = imu.accelCount[2] * imu.aRes;
  acc_mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

  switch(power_state){
    case OFF:
      tft.fillScreen(TFT_BLACK);
      if (!power_input && power_previous){
        power_state = ALWAYS_ON;
      }
      else if (acc_mag > 1){
        power_state = ON;        
        power_timer = millis();        
      }      
      break;
    case ON:
      if (!power_input && power_previous){
        power_state = ALWAYS_ON;
        break;
      }
      else if (acc_mag > 1){
        power_timer = millis();
      }
      else if (millis() - power_timer > 15000){
        power_state = OFF;
        break;
      }
    case ALWAYS_ON:
      if (!power_input && power_previous){
        power_state = ON;
        power_timer = millis();
      }
      switch(style_state){
        case HOUR_MINUTE:  
          if (!style_input && style_previous){
            style_state = HOUR_MINUTE_SECOND;
            tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
            tft_time();
          }        
          break; //don't forget break statements
        case HOUR_MINUTE_SECOND:
          if (!style_input && style_previous){
            style_state = HOUR_MINUTE;
            tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
            tft_time();
          }
      }
  }
  if (millis() - query_timer > 60000){
    sprintf(request_buffer, "GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
    strcat(request_buffer, "Host: iesc-s3.mit.edu\r\n"); //add more to the end
    strcat(request_buffer, "\r\n"); //add blank line!
    //submit to function that performs GET.  It will return output using response_buffer char array
    do_http_GET("iesc-s3.mit.edu", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    strncpy(hm, &response_buffer[11], 5); 
    strncpy(hms, &response_buffer[11], 8);
    if (get_time(hm, 0) > 12){
      hm[5] = 'p';
      hm[6] = 'm';
      hms[8] = 'p';
      hms[9] = 'm';
    }
    else {
      hm[5] = 'a';
      hm[6] = 'm';
      hms[8] = 'a';
      hms[9] = 'm';
    }
    normalize_hour(hm); 
    normalize_hour(hms);
    Serial.println(response_buffer);
    Serial.println(hm);
    Serial.println(hms);
    tft_time();
    query_timer = millis();
    blink_timer = millis(); // we also need to reset the blink timer so no drifting
  }
  if (millis() - blink_timer > 1000){
    if (hm[2] == ':'){
      hm[2] = ' ';
    }
    else{
      hm[2] = ':';
    }
    int second = get_time(hms, 6);
    int minute = get_time(hms, 3);
    int hour = get_time(hms, 0);
    second += 1;
    if (second >= 60){
      minute += 1;
      second = 0;
    }
    if (minute >= 60){
      hour += 1;
      minute = 0;
    }
    if (hour >= 13){
      hour = 1;
      if (hm[6] == 'a'){
        hm[5] = 'p';
        hms[8] = 'p';
      }
      else {
        hm[5] = 'a';
        hms[8] = 'a';
      }
    }
    print_time(hms, hour, 0);
    print_time(hms, minute, 3);
    print_time(hms, second, 6);
    print_time(hm, hour, 0);
    print_time(hm, minute, 3);
    tft_time();
    blink_timer = millis();
  }
  style_previous = style_input;
  power_previous = power_input;
}

int get_time(char* time, int pos){
  return (time[pos] - '0') * 10 + (time[pos + 1] - '0');
}

void print_time(char* time, int num, int pos){
  uint8_t tens_digit = floor(num / 10);
  uint8_t ones_digit = num % 10;
  time[pos] = tens_digit + '0';
  time[pos + 1] = ones_digit + '0';
}

void normalize_hour(char* time){
  int num = get_time(time, 0);
  if (num > 12){  // Convert to American time
    num -= 12;
  }
  print_time(time, num, 0);
}

void tft_time(){
  if (power_state == ALWAYS_ON || power_state == ON){
    tft.setCursor(0, 0, 1);
    if (style_state == HOUR_MINUTE){
      tft.setTextSize(3.5);
      if (hm[0] == '0'){
        hm[0] = ' ';
        tft.println(hm);
        hm[0] = '0';
      }
      else tft.println(hm);
    }
    else{
      tft.setTextSize(2.5);
      if (hms[0] == '0'){
        hms[0] = ' ';
        tft.println(hms);
        hms[0] = '0';
      }
      else tft.println(hms);
    }        
  }
}


/*----------------------------------
 * char_append Function:
 * Arguments:
 *    char* buff: pointer to character array which we will append a
 *    char c: 
 *    uint16_t buff_size: size of buffer buff
 *    
 * Return value: 
 *    boolean: True if character appended, False if not appended (indicating buffer full)
 */
uint8_t char_append(char* buff, char c, uint16_t buff_size){
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}

/*----------------------------------
 * do_http_GET Function:
 * Arguments:
 *    char* host: null-terminated char-array containing host to connect to
 *    char* request: null-terminated char-arry containing properly formatted HTTP GET request
 *    char* response: char-array used as output for function to contain response
 *    uint16_t response_size: size of response buffer (in bytes)
 *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
 *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
 * Return value:
 *    void (none)
 */
void do_http_GET(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}        
