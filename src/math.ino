#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <string.h>
#include <ArduinoJson.h>
#include "Button.h"

//TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN = 45;
const int LOOP_PERIOD = 40;

char network[] = "MIT"; // SSID for 6.08 Lab
char password[] = "";	// Password for 6.08 Lab
// Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000;	   // ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; // size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE];		   // char array buffer to hold HTTP request

char question[100];
int answer;
char input[6];
int inputIndex = 0;
char numbers[20] = "0123456789";
int simpleInput = 0;
int x, y, operation, const1, const2, const3, power1, power2, power3;
char operationString[100];
char expression[100];
char urlexpression[100];

const int BUTTON_TIMEOUT = 1000; // button timeout in milliseconds
const float ZOOM = 1;
const uint8_t BUTTON1 = 45; // pin connected to button
const uint8_t BUTTON2 = 39; // pin connected to button
const uint8_t BUTTON3 = 34; // pin connected to button
// button states
const uint8_t UP = 0;
const uint8_t BUTTONPRESSED = 1;
int button1_state = 0;
int button2_state = 0;
int button3_state = 0;
boolean button1_click = false;
boolean button2_click = false;
boolean button3_click = false;
boolean button4_click = false;

// difficulty states
const uint8_t EASYMATH = 0;
const uint8_t NORMALMATH = 1;
const uint8_t HARDMATH = 2;
const uint8_t INSANEMATH = 3;
int math_difficulty = 0;
static const char *math_string[] =
	{"EASYMATH", "NORMALMATH", "HARDMATH", "INSANEMATH"};
Button button45(45);
Button button39(39);
Button button38(38);
Button button34(34);

// interface states
const uint8_t OFF = 0;
const uint8_t START = 1;
const uint8_t SOLVING = 2;
int state = 0;

// void setup()
// {

// 	tft.init();								// init screen
// 	tft.setRotation(2);						// adjust rotation
// 	tft.setTextSize(1);						// default font size
// 	tft.fillScreen(TFT_BLACK);				// fill background
// 	tft.setTextColor(TFT_GREEN, TFT_BLACK); // set color of font to green foreground, black background
// 	tft.setCursor(0, 0, 2);
// 	Serial.begin(115200); // begin serial comms
// 	delay(100);			  // wait a bit (100 ms)
// 	Wire.begin();
// 	delay(50); // pause to make sure comms get set up

// 	Serial.begin(115200); // start serial at 115200 baud
// 	while (!Serial)
// 		;						   // wait for serial to start
// 	WiFi.begin(network, password); // attempt to connect to wifi
// 	uint8_t count = 0;			   // count used for Wifi check times
// 	Serial.print("Attempting to connect to ");
// 	Serial.println(network);
// 	while (WiFi.status() != WL_CONNECTED && count < 12)
// 	{
// 		delay(500);
// 		Serial.print(".");
// 		count++;
// 	}
// 	delay(2000);
// 	if (WiFi.isConnected())
// 	{ // if we connected then print our IP, Mac, and SSID we're on
// 		Serial.println("CONNECTED!");
// 		Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
// 					  WiFi.localIP()[1], WiFi.localIP()[0],
// 					  WiFi.macAddress().c_str(), WiFi.SSID().c_str());
// 		delay(500);
// 	}
// 	else
// 	{ // if we failed to connect just Try again.
// 		Serial.println("Failed to Connect :/  Going to restart");
// 		Serial.println(WiFi.status());
// 		ESP.restart(); // restart the ESP (proper way)
// 	}
// 	pinMode(45, INPUT_PULLUP); // first button
// 	pinMode(39, INPUT_PULLUP); // second button
// 	pinMode(38, INPUT_PULLUP); // third button
// 	pinMode(34, INPUT_PULLUP); // fourth button
// }

// void loop()
// {
// 	int math = math_loop();
// 	if (math != -1)
// 	{
// 		Serial.println(math);
// 	}
// }

int math_loop()
{

	button1_click = button45.update() != 0;
	button2_click = button39.update() != 0;
	button3_click = button34.update() != 0;
	button4_click = button38.update() != 0;

	char output[1000];
	tft.setCursor(0, 0, 2);
	int mode = -1;
	switch (state)
	{
	case (OFF):
		sprintf(output, "difficulty level %d                                                                                                                                                                                                                                                               ", math_difficulty);
		tft.println(output);
		if (button3_click)
		{
			state = START;
		}
		if (button1_click)
		{
			updateMathDifficulty();
		}
		break;
	case (START):
	{
		simpleInput = 0;
		switch (math_difficulty)
		{
		case (EASYMATH):
		{
			x = rand() % 10 + 1;
			y = rand() % 10 + 1;
			operation = rand() % 4;
			if (operation == 0)
			{
				answer = x + y;
				sprintf(question, "%d + %d = ", x, y);
			}
			else if (operation == 1)
			{
				if (x < y)
				{
					int temp = x;
					x = y;
					y = temp;
				}
				answer = y - x;
				sprintf(question, "%d - %d = ", x, y);
			}
			else if (operation == 2)
			{
				answer = x * y;
				sprintf(question, "%d * %d = ", x, y);
			}
			else if (operation == 3)
			{
				answer = x;
				x = x * y;
				sprintf(question, "%d / %d = ", x, y);
			}
			break;
		}
		case (NORMALMATH):
		{
			x = rand() % 10 + 11;
			y = rand() % 10 + 1;
			operation = rand() % 4;
			if (operation == 0)
			{
				answer = x + y;
				sprintf(question, "%d + %d = ", x, y);
			}
			else if (operation == 1)
			{
				if (x < y)
				{
					int temp = x;
					x = y;
					y = temp;
				}
				answer = y - x;
				sprintf(question, "%d - %d = ", x, y);
			}
			else if (operation == 2)
			{
				answer = x * y;
				sprintf(question, "%d * %d = ", x, y);
			}
			else if (operation == 3)
			{
				answer = x;
				x = x * y;
				sprintf(question, "%d / %d = ", x, y);
			}
			break;
		}
		case (HARDMATH):
		{
			x = rand() % 20 + 11;
			y = rand() % 20 + 10;
			operation = rand() % 4;
			if (operation == 0)
			{
				answer = x + y;
				sprintf(question, "%d + %d = ", x, y);
			}
			else if (operation == 1)
			{
				if (x < y)
				{
					int temp = x;
					x = y;
					y = temp;
				}
				answer = y - x;
				sprintf(question, "%d - %d = ", x, y);
			}
			else if (operation == 2)
			{
				answer = x * y;
				sprintf(question, "%d * %d = ", x, y);
			}
			else if (operation == 3)
			{
				answer = x;
				x = x * y;
				sprintf(question, "%d / %d = ", x, y);
			}
			break;
		}

		case (INSANEMATH):
		{
			operation = rand() % 2;
			// operation = 0;
			x = rand() % 5 + 1;
			const1 = rand() % 10;
			const2 = rand() % 10;
			const3 = rand() % 10;
			power1 = rand() % 3 + 2;
			power2 = rand() % 3 + 1;
			power3 = rand() % 3;

			if (operation == 0)
			{ // derivation
				sprintf(operationString, "%s", "derive");
				sprintf(expression, "%dx^%d + %dx^%d + %dx^%d ", const1, power1, const2, power2, const3, power3);
				sprintf(urlexpression, "%%28%ix%%5E%i%%29%%2B%%28%ix%%5E%i%%29%%2B%%28%ix%%5E%i%%29", const1, power1, const2, power2, const3, power3); // i promise this monster theoretically works
				sprintf(question, "for x=%d:            \nderive %s = ", x, expression);
				char request_buffer[500];
				sprintf(request_buffer, "GET /sandbox/sc/team41/newton.py?operation=%s&expression=%s&value=%d HTTP/1.1\r\n", operationString, urlexpression, x);
				strcat(request_buffer, "Host: 608dev-2.net\r\n");
				strcat(request_buffer, "\r\n"); // new line from header to body
				do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
				Serial.println("-----------");
				Serial.println(response);
				Serial.println("-----------");
				// char *start = strchr(response, '{');
				// char *end = strrchr(response, '}');
				// DynamicJsonDocument doc(1024);
				// deserializeJson(doc, start, end - start + 1);
				answer = atoi(response);
				Serial.println(answer);
			}
			else
			{ // integration
				sprintf(operationString, "integrate");
				sprintf(expression, "%dx^%d + %dx^%d + %dx^%d ", const1, power1, const2, power2, const3, power3);
				sprintf(question, "ignore constant term and round down  \nfor x=%d:           \nintegrate %s = ", x, expression);
				sprintf(urlexpression, "%%28%ix%%5E%i%%29%%2B%%28%ix%%5E%i%%29%%2B%%28%ix%%5E%i%%29", const1, power1, const2, power2, const3, power3); // i promise this monster theoretically works
				char request_buffer[500];
				sprintf(request_buffer, "GET /sandbox/sc/team41/newton.py?operation=%s&expression=%s&value=%d HTTP/1.1\r\n", operationString, urlexpression, x);
				strcat(request_buffer, "Host: 608dev-2.net\r\n");
				strcat(request_buffer, "\r\n"); // new line from header to body
				do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
				Serial.println("-----------");
				Serial.println(response);
				Serial.println("-----------");
				// char *start = strchr(response, '{');
				// char *end = strrchr(response, '}');
				// DynamicJsonDocument doc(1024);
				// deserializeJson(doc, start, end - start + 1);
				answer = atoi(response);
				Serial.println(answer);
			}
			break;
		}
		}
		sprintf(output, question);
		sprintf(input, "0000");
		inputIndex = 0;
		tft.println(output);
		state = SOLVING;
		break;
	}
	case (SOLVING):
	{
		// if (button1_click)
		// 	simpleInput++;
		// if (button2_click && simpleInput >= 0)
		// 	simpleInput--;
		if (button1_click)
		{
			input[3 - inputIndex] = (((input[3 - inputIndex] - '0') + 1) % 10 + '0');
		}
		if (button2_click)
		{
			inputIndex = (inputIndex + 1) % 4;
		}

		char blink[6];
		for (int i = 0; i < 4; i++)
		{
			if (i == 3 - inputIndex)
			{
				blink[i] = ' ';
			}
			else
			{
				blink[i] = input[i];
			}
		}
		if (button3_click)
		{
			if (atoi(input) == answer)
			{
				state = OFF;
				mode = math_difficulty;
			}
			else
			{
				state = START;
			}
		}
		if (millis() % 1000 < 500)
			sprintf(output, "%s%s                                                                                                          ", question, input);
		else
			sprintf(output, "%s%s                                                                                                          ", question, blink);
		tft.println(output);
		break;
	}
	}
	return mode;
}
void updateMathDifficulty()
{
	math_difficulty = (math_difficulty + 1) % 4;
}
