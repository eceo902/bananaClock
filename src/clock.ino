const uint8_t OFF = 0;
const uint8_t ON = 1;
const uint8_t ALWAYS_ON = 2;

const uint8_t HOUR_MINUTE = 0;
const uint8_t HOUR_MINUTE_SECOND = 1;

const uint8_t STANDARD = 0;
const uint8_t MILITARY = 1;

char hm[8];
char hms[11];
char hm_military[6];
char hms_military[9];
double acc_magg;

uint8_t power_state;
uint8_t style_state;
uint8_t military_state;

unsigned long query_timer;
unsigned long power_timer;
unsigned long blink_timer;
unsigned long weather_timer;
unsigned long location_timer;

const char PREFIX[] = "{\"wifiAccessPoints\": ["; //beginning of json body
const char SUFFIX[] = "]}"; //suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; //don't change this and don't share this
const char WEATHER_API_KEY[] = "49c9ca83af65274dfefed933ba2ba723";
const int MAX_APS = 5;

char request[3500];
char google_json_body[3000];
uint32_t time_since_sample;      // used for microsecond timing
uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.
char*  SERVER = "googleapis.com";  // Server URL
uint32_t timer;
char units[10] = "Imperial";
char weather[50];
char temp[50];
double lat;
double lon;

void setup_clock() {   // this is called when transitioning to clock state
  tft.setRotation(1); //adjust rotation
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  tft.setTextSize(0.5);
  tft.setCursor(0, 90, 1);
  tft.println("Buttons\n1-Seconds\n2-Always On\n3-Military");

  power_state = ALWAYS_ON;
  style_state = HOUR_MINUTE;
  military_state = STANDARD;

  calibrate_time();


  query_timer = millis();
  blink_timer = millis(); // we also need to reset the blink timer so no drifting
  weather_timer = millis();
  location_timer = millis();
  get_location();
  get_weather();
}

char* loop_clock() {   // this is called when we remain in the clock state

  int style_input = bv45;
  int power_input = bv39;
  int military_input = bv38;
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  z = imu.accelCount[2] * imu.aRes;
  acc_magg = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  if(millis() - weather_timer > 300000){ 
    weather_timer = millis();
    get_weather();    
  }
  if(millis() - location_timer > 500000){
    location_timer = millis();
    get_location();    
  }

  switch(military_state) {
    case STANDARD:
      if (military_input != 0) {
        military_state = MILITARY;
        tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
        print_time();
        Serial.println("Switched to standard time");
        tft.setTextSize(1);
        tft.println(weather);
        tft.println(temp);
        
      }
      break;
    case MILITARY:
      if (military_input != 0) {
        military_state = STANDARD;
        tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
        Serial.println("Switched to military time");
        print_time();
        tft.setTextSize(1);
        tft.println(weather);
        tft.println(temp);
      }
  }

  switch(style_state) {
    case HOUR_MINUTE:  
      if (style_input != 0) {
        style_state = HOUR_MINUTE_SECOND;
        tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
        print_time();
        tft.setTextSize(1);
        tft.println(weather);
        tft.println(temp);
      }        
      break; //don't forget break statements
    case HOUR_MINUTE_SECOND:
      if (style_input != 0) {
        style_state = HOUR_MINUTE;
        tft.fillScreen(TFT_BLACK); //need to modify tft so that we get instant feedback on button click
        print_time();
        tft.setTextSize(1);
        tft.println(weather);
        tft.println(temp);
      }
  }

  switch(power_state) {
    case OFF:
      tft.fillScreen(TFT_BLACK);
      if (power_input != 0) {
        power_state = ALWAYS_ON;
      }
      else if (acc_magg > 0.5) {
        power_state = ON;        
        power_timer = millis();        
      }      
      break;
    case ON:
      if (power_input != 0) {
        power_state = ALWAYS_ON;
      }
      else if (acc_magg > 0.5) {
        power_timer = millis();
      }
      else if (millis() - power_timer > 15000) {
        power_state = OFF;
      }
      break;
    case ALWAYS_ON:
      if (power_input != 0) {
        power_state = ON;
        power_timer = millis();
      }
  }
  if (millis() - query_timer > 60000) {
    calibrate_time();
  }
  if (millis() - blink_timer > 1000) {
    if (hm[2] == ':') {
      hm[2] = ' ';
      hm_military[2] = ' ';
    }
    else{
      hm[2] = ':';
      hm_military[2] = ':';
    }
    int second = get_time(hms, 6);
    int minute = get_time(hms, 3);
    int hour = get_time(hms, 0);
    int hour_military = get_time(hms_military, 0);
    second += 1;
    if (second >= 60) {
      minute += 1;
      second = 0;
    }
    if (minute >= 60) {
      hour += 1;
      hour_military += 1;
      minute = 0;
    }
    if (hour >= 13) {
      hour = 1;
      if (hm[6] == 'a') {
        hm[5] = 'p';
        hms[8] = 'p';
      }
      else {
        hm[5] = 'a';
        hms[8] = 'a';
      }
    }
    time_to_string(hms, hour, 0);
    time_to_string(hms, minute, 3);
    time_to_string(hms, second, 6);
    time_to_string(hm, hour, 0);
    time_to_string(hm, minute, 3);

    if (hour_military >= 24) {
      hour_military = 0;
    }
    time_to_string(hms_military, hour_military, 0);
    time_to_string(hms_military, minute, 3);
    time_to_string(hms_military, second, 6);
    time_to_string(hm_military, hour_military, 0);
    time_to_string(hm_military, minute, 3);

    print_time();
    blink_timer = millis();
  }
  return hm_military;
}

void calibrate_time() {
  sprintf(request_buffer, "GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
  strcat(request_buffer, "Host: iesc-s3.mit.edu\r\n"); //add more to the end
  strcat(request_buffer, "\r\n"); //add blank line!
  //submit to function that performs GET.  It will return output using response_buffer char array
  do_http_request("iesc-s3.mit.edu", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  strncpy(hm, &response_buffer[11], 5);
  strncpy(hms, &response_buffer[11], 8);
  strncpy(hm_military, &response_buffer[11], 5);
  strncpy(hms_military, &response_buffer[11], 8);
  if (get_time(hm, 0) > 12) {
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
  print_time();
  query_timer = millis();
  blink_timer = millis(); // we also need to reset the blink timer so no drifting
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

void normalize_hour(char* time) {
  int num = get_time(time, 0);
  if (num > 12) {  // Convert to American time
    num -= 12;
  }
  else if (num == 0) {
    num = 12;
  }
  time_to_string(time, num, 0);
}

void print_time() {
  if (power_state == ALWAYS_ON || power_state == ON) {
    tft.setCursor(6, 40, 1);
    if (military_state == STANDARD) {
      if (style_state == HOUR_MINUTE) {
        tft.setTextSize(3.5);
        tft.println(hm);
      }
      else{
        tft.setTextSize(2.5);
        tft.println(hms);
      }  
    }    
    else {
      if (style_state == HOUR_MINUTE) {
        tft.setTextSize(5);
        tft.println(hm_military);
      }
      else{
        tft.setTextSize(3);
        tft.println(hms_military);
      }       
    }  
  }
} 

void get_location(){
int offset = sprintf(google_json_body, "%s", PREFIX);
    int n = WiFi.scanNetworks(); //run a new scan. could also modify to use original scan from setup so quicker (though older info)
    // Serial.println("scan done");
    if (n == 0) {
      Serial.println("no networks found");
    } else {
      int max_aps = max(min(MAX_APS, n), 1);
      for (int i = 0; i < max_aps; ++i) { //for each valid access point
        uint8_t* mac = WiFi.BSSID(i); //get the MAC Address
        offset += wifi_object_builder(google_json_body + offset, 3500-offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); //generate the query
        if(i!=max_aps-1){
          offset +=sprintf(google_json_body+offset,",");//add comma between entries except trailing.
        }
      }
      sprintf(google_json_body + offset, "%s", SUFFIX);
      // Serial.println(google_json_body);
      int len = strlen(google_json_body);
      // Make a HTTP request:
      // Serial.println("SENDING REQUEST");
      request[0] = '\0'; //set 0th byte to null
      offset = 0; //reset offset variable for sprintf-ing
      offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
      offset += sprintf(request + offset, "Host: googleapis.com\r\n");
      offset += sprintf(request + offset, "Content-Type: application/json\r\n");
      offset += sprintf(request + offset, "cache-control: no-cache\r\n");
      offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
      offset += sprintf(request + offset, "%s\r\n", google_json_body);
      do_https_request(SERVER, request, response, 1000, RESPONSE_TIMEOUT, false);
      // Serial.println("-----------");
      // Serial.println(response);
      // Serial.println("-----------");
      //For Part Two of Lab04B, you should start working here. Create a DynamicJsonDoc of a reasonable size (few hundred bytes at least...)
      char* start = strchr(response, '{');
      char* end = strrchr(response, '}');

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, start, end-start+1);
      lat = doc["location"]["lat"];
      lon = doc["location"]["lng"];
  }
}


// //https://openweathermap.org/current
//https://openweathermap.org/current
void get_weather(){
    
      char str[100];
      sprintf(str, "lat %f, lon %f", lat, lon);
      // Serial.println(str);
      char request_buffer[500];
			sprintf(request_buffer, "GET /data/2.5/weather?lat=%f&lon=%f&appid=%s&units=%s HTTP/1.1\r\n", lat, lon, WEATHER_API_KEY, units);
			strcat(request_buffer, "Host: api.openweathermap.org\r\n");
			strcat(request_buffer, "\r\n"); // new line from header to body
			do_http_request("api.openweathermap.org", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      // Serial.println(request_buffer);
			// Serial.println("-----------");
			// Serial.println(response);
			// Serial.println("-----------");

      char* start = strchr(response, '{');
      char* end = strrchr(response, '}');
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, start, end-start+1);
      sprintf(weather, doc["weather"][0]["main"]);
      char str_temp[10];
      dtostrf(doc["main"]["temp"], 1, 1, str_temp);
      sprintf(temp, "%s%c", str_temp,248); //weird thing is degree symbol 
      //sprintf(temp, atoi(doc["main"]["temp"]));
      Serial.println(weather);
      Serial.println(temp);
}