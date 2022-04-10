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