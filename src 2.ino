#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <string.h>

TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN = 45;
const int LOOP_PERIOD = 40;

MPU6050 imu; //imu object called, appropriately, imu

char network[] = "MIT";  //SSID for 6.08 Lab
char password[] = ""; //Password for 6.08 Lab


//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char old_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

uint32_t primary_timer;

char table[] = "AAADAFAGAVAXDADDDFDGDVDXFAFDFFFGFVFXGAGDGFGGGVGXVAVDVFVGVVVXXAXDXFXGXVXX";
char entries[] = "N13ACH8TBMO2E5RWPD4F67GI9J0QLKSUZYVX";

const int key_length = 10;
char key[key_length] = "SPIDERMAN";
int key_order[key_length - 1];
int reverse_order[key_length - 1];


int old_val;

//used to get x,y values from IMU accelerometer!
void get_angle(float* x, float* y) {
  imu.readAccelData(imu.accelCount);
  *x = imu.accelCount[0] * imu.aRes;
  *y = imu.accelCount[1] * imu.aRes;
}

void lookup(char* query, char* response, int response_size) {
  char request_buffer[200];
  //CHANGE WHERE THIS IS TARGETED! IT SHOULD TARGET YOUR SERVER SCRIPT
  //CHANGE WHERE THIS IS TARGETED! IT SHOULD TARGET YOUR SERVER SCRIPT
  //CHANGE WHERE THIS IS TARGETED! IT SHOULD TARGET YOUR SERVER SCRIPT
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/eseeyave/adfgvx/adfgvx.py?subject=%s HTTP/1.1\r\n", query);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, response, response_size, RESPONSE_TIMEOUT, true);
}

//enum for button states
enum button_state {S0, S1, S2, S3, S4};

class Button {
	public:
	uint32_t S2_start_time;
	uint32_t button_change_time;    
	uint32_t debounce_duration;
	uint32_t long_press_duration;
	uint8_t pin;
	uint8_t flag;
	uint8_t button_pressed;
	button_state state; // This is public for the sake of convenience
	Button(int p) {
	flag = 0;  
		state = S0;
		pin = p;
		S2_start_time = millis(); //init
		button_change_time = millis(); //init
		debounce_duration = 10;
		long_press_duration = 1000;
		button_pressed = 0;
	}
	void read() {
		uint8_t button_val = digitalRead(pin);  
		button_pressed = !button_val; //invert button
	}
	int update() {
		read();
		flag = 0;
		if (state==S0) {
			if (button_pressed) {
			state = S1;
			button_change_time = millis();
			}
		} 
		else if (state==S1) {
			if (millis() - button_change_time > debounce_duration) {
			state = S2;
			S2_start_time = millis();
			}
			else if (!button_pressed) {
			state = S0;
			button_change_time = millis();
			}
		} 
		else if (state==S2) {
			if (millis() - S2_start_time > long_press_duration) {
			state = S3;
			}
			else if (!button_pressed) {
			state = S4;
			button_change_time = millis();
			}
		} 
		else if (state==S3) {
			if (!button_pressed) {
			state = S4;
			button_change_time = millis();
			}
		} 
		else if (state==S4) {      	
			if (millis() - button_change_time > debounce_duration) {
			if (millis() - S2_start_time > long_press_duration) {
				flag = 2;
			}
			else {
				flag = 1;
			}
			state = S0;
			}
			else if (button_pressed) {
			if (millis() - S2_start_time > long_press_duration) {
				state = S3;
			}
			else {
				state = S2;
			}
			button_change_time = millis();
			}
		}
		return flag;
	}
};

class WikipediaGetter {
    char alphabet[50] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char msg[400]; //contains previous query response
    char query_string[50];
    int char_index;
    int state;
    uint32_t scroll_timer;
    const int scroll_threshold = 150;
    const float angle_threshold = 0.35;
  public:

    WikipediaGetter() {
      state = 0;
      memset(msg, 0, 400);//empty it.
	  memset(query_string, 0, 50);
      strcat(msg, "Long Press to Start!");
      char_index = 0;
      scroll_timer = millis();
    }
    void update(float angle, int button, char* output) {
		switch(state) {
		case 0:
			memset(output, 0, sizeof(output));
			strcat(output, msg);
			if (button == 2) {
				state = 1;
				scroll_timer = millis();
				memset(output, 0, 1000);
				memset(query_string, 0, sizeof(query_string));
				char_index = 0;
			}
			break;
		case 1:
			if (button == 1) {
				query_string[strlen(query_string)] = alphabet[char_index];
				char_index = 0;
				Serial.println(output);
				Serial.println(output + 7);
				Serial.println(query_string);
			}
			else if (button == 2) {
				state = 2;
				memset(output, 0, sizeof(output));
				break;
			}
			else if (millis() - scroll_timer > scroll_threshold) {
				if (angle > angle_threshold) {
					char_index++;
					if (char_index > strlen(alphabet) - 1) char_index = 0;
					scroll_timer = millis();
				}
				else if (angle < -angle_threshold) {
					char_index--;
					if (char_index < 0) char_index = strlen(alphabet) - 1;
					scroll_timer = millis();
				}
			}
			memset(output, 0, 1000);
			strcat(output, query_string);
			output[strlen(output)] = alphabet[char_index];
			break;
		case 2:
			strcat(output, "Sending Query");
			state = 3;
			break;
		case 3:
			char encrypted_string[100] = {0};
			encrypt(query_string, encrypted_string);
			strcat(encrypted_string, "&len=200");

			char temp_string[400] = {0};
			lookup(encrypted_string, temp_string, sizeof(temp_string));

			memset(msg, 0, sizeof(msg));
			decrypt(temp_string, msg);
			memset(output, 0, sizeof(output));
			strcat(output, msg);
			state = 0;
		}
	}
};

WikipediaGetter wg; //wikipedia object
Button button(BUTTON_PIN); //button object!


void setup() {
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

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
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background

  pinMode(BUTTON_PIN, INPUT_PULLUP);

	char key_copy[key_length];
	sprintf(key_copy, key);
	
	for (int i = 0; i < strlen(key_copy); i++) {
		uint8_t min_value = 255;
		int min_index;
		for (int j = 0; j < strlen(key_copy); j++) {
			if (key_copy[j] < min_value) {
				min_value = key_copy[j];
				min_index = j;
			}
		}
		key_order[i] = min_index;
		key_copy[min_index] = 255;
		Serial.println(min_index);
	}

	for (int i = 0; i < strlen(key); i++) {
		reverse_order[key_order[i]] = i;
	}

  primary_timer = millis();
}

void loop() {
  float x, y;
  get_angle(&x, &y); //get angle values
  int bv = button.update(); //get button value
  wg.update(y, bv, response); //input: angle and button, output String to display on this timestep
  if (strcmp(response, old_response) != 0) {//only draw if changed!
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 1);
    tft.println(response);
  }
  memset(old_response, 0, sizeof(old_response));
  strcat(old_response, response);
  while (millis() - primary_timer < LOOP_PERIOD); //wait for primary timer to increment
  primary_timer = millis();
}


void encrypt(char* plaintext, char* ciphertext) {
	Serial.printf("Query: %s\n", plaintext);
	char intermediatetext[100] = {0};
	for (int i = 0; i < strlen(plaintext); i++) {
		int j = 0;
		while (entries[j] != plaintext[i]) j++;
		intermediatetext[2 * i] = table[2 * j];
		intermediatetext[2 * i + 1] = table[2 * j + 1];
	}
	Serial.printf("Intermediate text before sorting: %s\n", intermediatetext);
	for (int i = 0; i < strlen(key); i++) {
		float count = 0;
		for (int j = key_order[i]; j < strlen(intermediatetext); j += strlen(key)) {
			ciphertext[strlen(ciphertext)] = intermediatetext[j];
			count ++;
		}
		if (count < ((float) strlen(intermediatetext) / (float) strlen(key))) ciphertext[strlen(ciphertext)] = ' ';
	}
	Serial.printf("Ciphertext: %s\n", ciphertext);
}

void decrypt(char* ciphertext, char* plaintext) {
	Serial.printf("Response ciphertext: %s\n", ciphertext);
	char intermediatetext[400] = {0};
	int fraction = strlen(ciphertext) / strlen(key);
	for (int i = 0; i < fraction; i++) {
		for (int j = 0; j < strlen(key); j++) {
			if (ciphertext[reverse_order[j] * fraction + i] != ' ') {
				intermediatetext[strlen(intermediatetext)] = ciphertext[reverse_order[j] * fraction + i];
			}
		}
	}

	for (int i = 0; i < strlen(intermediatetext); i += 2) {
		int j = 0;
		while (!(table[j] == intermediatetext[i] && table[j+1] == intermediatetext[i+1])) j += 2;
		plaintext[strlen(plaintext)] = entries[j / 2];
	}
	Serial.printf("Response plaintext: %s\n", plaintext);
}



/*----------------------------------
  char_append Function:
  Arguments:
     char* buff: pointer to character array which we will append a
     char c:
     uint16_t buff_size: size of buffer buff

  Return value:
     boolean: True if character appended, False if not appended (indicating buffer full)
*/
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

/*----------------------------------
   do_http_request Function:
   Arguments:
      char* host: null-terminated char-array containing host to connect to
      char* request: null-terminated char-arry containing properly formatted HTTP request
      char* response: char-array used as output for function to contain response
      uint16_t response_size: size of response buffer (in bytes)
      uint16_t response_timeout: duration we'll wait (in ms) for a response from server
      uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
   Return value:
      void (none)
*/
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}