#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <string.h>
#include <ArduinoJson.h>
#include "Button.h"

//TFT_eSPI tft = TFT_eSPI();
// const int SCREEN_HEIGHT = 160;
// const int SCREEN_WIDTH = 128;
// const int BUTTON_PIN = 45;
// const int LOOP_PERIOD = 40;

// char network[] = "MIT"; // SSID for 6.08 Lab
// char password[] = "";	// Password for 6.08 Lab
// // Some constants and some resources:
// const int RESPONSE_TIMEOUT = 6000;	   // ms to wait for response from host
// const uint16_t OUT_BUFFER_SIZE = 1000; // size of buffer to hold HTTP response

Button mathbutton45(45);
Button mathbutton39(39);
Button mathbutton38(38);
Button mathbutton34(34);

char question[100];
int answer;
char input[6];
int inputIndex = 0;
char numbers[20] = "0123456789";
int simpleInput = 0;
int mathx, mathy, operation, const1, const2, const3, power1, power2, power3;
char operationString[100];
char expression[100];
char urlexpression[100];

boolean mathbutton1_click = false;
boolean mathbutton2_click = false;
boolean mathbutton3_click = false;
boolean mathbutton4_click = false;

// difficulty states
const uint8_t EASYMATH = 0;
const uint8_t NORMALMATH = 1;
const uint8_t HARDMATH = 2;
const uint8_t INSANEMATH = 3;
int math_difficulty = 0;
static const char *math_string[] =
	{"EASY", "NORMAL", "HARD", "INSANE"};

// interface states
const uint8_t MATHOFF = 0;
const uint8_t MATHSTART = 1;
const uint8_t MATHSOLVING = 2;
int math_state = 0;


void math_setup(){s
	tft.setRotation(2);						// adjust rotation
	tft.setTextSize(1);						// default font size
	tft.fillScreen(TFT_BLACK);				// fill background
	tft.setTextColor(TFT_GREEN, TFT_BLACK); // set color of font to green foreground, black background
	tft.setCursor(0, 0, 2);
  math_state = 0;
  math_difficulty = 0;
}


int math_loop()
{

	mathbutton1_click = mathbutton45.update() != 0;
	mathbutton2_click = mathbutton39.update() != 0;
	mathbutton3_click = mathbutton34.update() != 0;
	mathbutton4_click = mathbutton38.update() != 0;

	char output[500];
	tft.setCursor(0, 0, 2);
	int mode = -1;
	switch (math_state)
	{
	case (MATHOFF):
		sprintf(output, "difficulty level %s                                                                                              ", math_string[math_difficulty]);
		tft.println(output);
		if (mathbutton3_click)
		{
			math_state = MATHSTART;
		}
		if (mathbutton1_click)
		{
			updateMathDifficulty();
		}
		break;
	case (MATHSTART):
	{
		simpleInput = 0;
		switch (math_difficulty)
		{
		case (EASYMATH):
		{
			mathx = rand() % 10 + 1;
			mathy = rand() % 10 + 1;
			operation = rand() % 4;
			if (operation == 0)
			{
				answer = mathx + mathy;
				sprintf(question, "%d + %d = ", mathx, mathy);
			}
			else if (operation == 1)
			{
				if (mathx < mathy)
				{
					int temp = mathx;
					mathx = mathy;
					mathy = temp;
				}
				answer = mathy - mathx;
				sprintf(question, "%d - %d = ", mathx, mathy);
			}
			else if (operation == 2)
			{
				answer = mathx * mathy;
				sprintf(question, "%d * %d = ", mathx, mathy);
			}
			else if (operation == 3)
			{
				answer = mathx;
				mathx = mathx * mathy;
				sprintf(question, "%d / %d = ", mathx, mathy);
			}
			break;
		}
		case (NORMALMATH):
		{
			mathx = rand() % 10 + 11;
			mathy = rand() % 10 + 1;
			operation = rand() % 4;
			if (operation == 0)
			{
				answer = mathx + mathy;
				sprintf(question, "%d + %d = ", mathx, mathy);
			}
			else if (operation == 1)
			{
				if (mathx < mathy)
				{
					int temp = mathx;
					mathx = mathy;
					mathy = temp;
				}
				answer = mathy - mathx;
				sprintf(question, "%d - %d = ", mathx, mathy);
			}
			else if (operation == 2)
			{
				answer = mathx * mathy;
				sprintf(question, "%d * %d = ", mathx, mathy);
			}
			else if (operation == 3)
			{
				answer = mathx;
				mathx = mathx * mathy;
				sprintf(question, "%d / %d = ", mathx, mathy);
			}
			break;
		}
		case (HARDMATH):
		{
			mathx = rand() % 20 + 11;
			mathy = rand() % 20 + 10;
			operation = rand() % 4;
			if (operation == 0)
			{
				answer = mathx + mathy;
				sprintf(question, "%d + %d = ", mathx, mathy);
			}
			else if (operation == 1)
			{
				if (mathx < mathy)
				{
					int temp = mathx;
					mathx = mathy;
					mathy = temp;
				}
				answer = mathy - mathx;
				sprintf(question, "%d - %d = ", mathx, mathy);
			}
			else if (operation == 2)
			{
				answer = mathx * mathy;
				sprintf(question, "%d * %d = ", mathx, mathy);
			}
			else if (operation == 3)
			{
				answer = mathx;
				mathx = mathx * mathy;
				sprintf(question, "%d / %d = ", mathx, mathy);
			}
			break;
		}

		case (INSANEMATH):
		{
			operation = rand() % 2;
			// operation = 0;
			mathx = rand() % 5 + 1;
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
				sprintf(request_buffer, "GET /sandbox/sc/team41/newton.py?operation=%s&expression=%s&value=%d HTTP/1.1\r\n", operationString, urlexpression, mathx);
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
				sprintf(question, "ignore constant term and round down  \nfor x=%d:           \nintegrate %s = ", mathx, expression);
				sprintf(urlexpression, "%%28%ix%%5E%i%%29%%2B%%28%ix%%5E%i%%29%%2B%%28%ix%%5E%i%%29", const1, power1, const2, power2, const3, power3); // i promise this monster theoretically works
				char request_buffer[500];
				sprintf(request_buffer, "GET /sandbox/sc/team41/newton.py?operation=%s&expression=%s&value=%d HTTP/1.1\r\n", operationString, urlexpression, mathx);
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
		// sprintf(prompt, question);
		sprintf(input, "0000\0");
		inputIndex = 0;
		tft.println(output);
		math_state = MATHSOLVING;
		//setup_joystick();
		break;
	}
	case (MATHSOLVING):
	{
		//THIS IS ALL FOR NO KEYBOARD
		if (mathbutton1_click)
		{
			//inputIndex = (inputIndex + 3) % 4;
			input[3 - inputIndex] = (((input[3 - inputIndex] - '0') + 1) % 10 + '0');
		}
		if (mathbutton2_click)
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
    blink[4]='\0';
		if (mathbutton4_click)
		{
			if (atoi(input) == answer)
			{
				math_state = MATHOFF;
				mode = math_difficulty;
			}
			else
			{
				math_state = MATHSTART;
			}
		}
    // Serial.println(question);
    // Serial.println(input);

		if (millis() % 1000 < 500)
			sprintf(output, "%s %s                                                                                                          ", question, input);
		else
			sprintf(output, "%s %s                                                                                                          ", question, blink);
		tft.println(output);

		// YES KEYBOARD

		// int change = loop_joystick();
		// if(change ==1){
		// 	if (atoi(input) == answer)
		// 	{
		// 		math_state = MATHOFF;
		// 		mode = math_difficulty;
		// 	}
		// 	else
		// 	{
		// 		math_state = MATHSTART;
		// 	}
		// }

		break;
	}
	}
	return mode;
}
void updateMathDifficulty()
{
	math_difficulty = (math_difficulty + 1) % 4;
}
