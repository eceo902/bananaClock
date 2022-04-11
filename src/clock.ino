const uint8_t OFF = 0;
const uint8_t ON = 1;
const uint8_t ALWAYS_ON = 2;

const uint8_t HOUR_MINUTE = 0;
const uint8_t HOUR_MINUTE_SECOND = 1;

char hm[6];
char hms[9];


uint8_t power_state;
uint8_t style_state;
unsigned long query_timer;
unsigned long power_timer;
unsigned long blink_timer;


void setup_clock() {   // this is called when transitioning to clock state
  power_state = ALWAYS_ON;
  style_state = HOUR_MINUTE;

  sprintf(request_buffer, "GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
  strcat(request_buffer, "Host: iesc-s3.mit.edu\r\n"); //add more to the end
  strcat(request_buffer, "\r\n"); //add blank line!
  //submit to function that performs GET.  It will return output using response_buffer char array
  do_http_request("iesc-s3.mit.edu", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  strncpy(hm, &response_buffer[11], 5);
  strncpy(hms, &response_buffer[11], 8);

  Serial.println(response_buffer);
  Serial.println(hm);
  Serial.println(hms);

  query_timer = millis();
  blink_timer = millis(); // we also need to reset the blink timer so no drifting
}

char* loop_clock() {   // this is called when we remain in the clock state
  int style_input = button45.update();
  int power_input = button39.update();
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  z = imu.accelCount[2] * imu.aRes;
  acc_mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

  switch(power_state) {
    case OFF:
      tft.fillScreen(TFT_BLACK);
      if (power_input != 0) {
        power_state = ALWAYS_ON;
      }
      else if (acc_mag > 0.5) {
        power_state = ON;        
        power_timer = millis();        
      }      
      break;
    case ON:
      if (power_input != 0) {
        power_state = ALWAYS_ON;
        break;
      }
      else if (acc_mag > 0.5) {
        power_timer = millis();
      }
      else if (millis() - power_timer > 15000) {
        power_state = OFF;
        break;
      }
    case ALWAYS_ON:
      if (power_input != 0) {
        power_state = ON;
        power_timer = millis();
      }
      switch(style_state) {
        case HOUR_MINUTE:  
          if (style_input != 0) {
            style_state = HOUR_MINUTE_SECOND;
            tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
            print_time();
          }        
          break; //don't forget break statements
        case HOUR_MINUTE_SECOND:
          if (style_input != 0) {
            style_state = HOUR_MINUTE;
            tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
            print_time();
          }
      }
  }
  if (millis() - query_timer > 60000) {
    sprintf(request_buffer, "GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
    strcat(request_buffer, "Host: iesc-s3.mit.edu\r\n"); //add more to the end
    strcat(request_buffer, "\r\n"); //add blank line!
    //submit to function that performs GET.  It will return output using response_buffer char array
    do_http_request("iesc-s3.mit.edu", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    strncpy(hm, &response_buffer[11], 5);
    strncpy(hms, &response_buffer[11], 8);

    Serial.println(response_buffer);
    Serial.println(hm);
    Serial.println(hms);
    print_time();
    query_timer = millis();
    blink_timer = millis(); // we also need to reset the blink timer so no drifting
  }
  if (millis() - blink_timer > 1000) {
    if (hm[2] == ':') {
      hm[2] = ' ';
    }
    else{
      hm[2] = ':';
    }
    int second = get_time(hms, 6);
    int minute = get_time(hms, 3);
    int hour = get_time(hms, 0);
    second += 1;
    if (second >= 60) {
      minute += 1;
      second = 0;
    }
    if (minute >= 60) {
      hour += 1;
      minute = 0;
    }
    if (hour >= 13) {
      hour = 1;
    }
    time_to_string(hms, hour, 0);
    time_to_string(hms, minute, 3);
    time_to_string(hms, second, 6);
    time_to_string(hm, hour, 0);
    time_to_string(hm, minute, 3);
    print_time();
    blink_timer = millis();
  }

  return hm;
}

int get_time(char* time, int pos) {
  return (time[pos] - '0') * 10 + (time[pos + 1] - '0');
}

void time_to_string(char* time, int num, int pos) {
  uint8_t tens_digit = floor(num / 10);
  uint8_t ones_digit = num % 10;
  time[pos] = tens_digit + '0';
  time[pos + 1] = ones_digit + '0';
}

void print_time() {
  if (power_state == ALWAYS_ON || power_state == ON) {
    tft.setCursor(8, 40, 1);
    if (style_state == HOUR_MINUTE) {
      tft.setTextSize(5);
      tft.println(hm);
    }
    else{
      tft.setTextSize(3);
      tft.println(hms);
    }        
  }
} 