#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <string.h>
#include <ArduinoJson.h>
#include "Button.h"

// TFT_eSPI tft = TFT_eSPI();
//  const int SCREEN_HEIGHT = 160;
//  const int SCREEN_WIDTH = 128;
//  const int BUTTON_PIN = 45;
//  const int LOOP_PERIOD = 40;

// char network[] = "MIT"; // SSID for 6.08 Lab
// char password[] = "";	// Password for 6.08 Lab
// // Some constants and some resources:
// const int RESPONSE_TIMEOUT = 6000;	   // ms to wait for response from host
// const uint16_t OUT_BUFFER_SIZE = 1000; // size of buffer to hold HTTP response

char quote[200];
int quoteLength;
char currquote[200];
int alphabet[27] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
int cipherinputIndex;
;
int shift;
const int keyLength = 6;
int key[keyLength];

// const int BUTTON_TIMEOUT = 1000; // button timeout in milliseconds
// const float ZOOM = 1;
// const uint8_t BUTTON1 = 45; // pin connected to button
// const uint8_t BUTTON2 = 39; // pin connected to button
// const uint8_t BUTTON3 = 34; // pin connected to button
// Button button45(45);
// Button button39(39);
// Button button38(38);
// Button button34(34);
// button states
// const uint8_t UP = 0;
// const uint8_t BUTTONPRESSED = 1;
// int button1_state = 0;
// int button2_state = 0;
// int button3_state = 0;
Button cipherbutton45(45);
Button cipherbutton39(39);
Button cipherbutton38(38);
Button cipherbutton34(34);
boolean cipherbutton1_click = false;
boolean cipherbutton2_click = false;
boolean cipherbutton3_click = false;
boolean cipherbutton4_click = false;

// interface states
const uint8_t CIPHEROFF = 0;
const uint8_t CIPHERSTART = 1;
const uint8_t CIPHERSOLVING = 2;
int cipher_state = 0;

const uint8_t EASYCIPHER = 0;
const uint8_t NORMALCIPHER = 1;
const uint8_t INSANECIPHER = 2;
// static const char *enum_str[] =
// 	{"EASYCIPHER", "NORMALCIPHER", "INSANECIPHER"};
int cipher_difficulty = 0;

void cipher_setup()
{
	//tft.init();								// init screen
	tft.setRotation(2);						// adjust rotation
	tft.setTextSize(1);						// default font size
	tft.fillScreen(TFT_BLACK);				// fill background
	tft.setTextColor(TFT_GREEN, TFT_BLACK); // set color of font to green foreground, black background
	tft.setCursor(0, 0, 1);
}
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
// 	iut cipher = cipher_loop();
// 	if (cipher != -1)
// 		Serial.println(cipher);
// }

int cipher_loop()
{

	cipherbutton1_click = cipherbutton45.update() != 0;
	cipherbutton2_click = cipherbutton39.update() != 0;
	cipherbutton3_click = cipherbutton34.update() != 0;
	cipherbutton4_click = cipherbutton38.update() != 0;

	char output[1000];
	tft.setCursor(0, 0, 1);
	int mode = -1;
	switch (cipher_state)
	{
	case (CIPHEROFF):
		sprintf(output, "difficulty level %d                                                                                                                                                                                                                                                                    ", cipher_difficulty);
		tft.println(output);
		if (cipherbutton3_click)
		{
			cipher_state = CIPHERSTART;
		}
		if (cipherbutton1_click)
		{
			updateCipherDifficulty();
		}
		break;
	case (CIPHERSTART):
	{
		quoteLength = get_quote(quote);
		for (int i = 0; i <= quoteLength; i++)
		{
			quote[i] = toupper(quote[i]);
		}

		switch (cipher_difficulty)
		{
		case (EASYCIPHER): // caesar shift
			shift = rand() % 25 + 1;
			for (int i = 0; i <= quoteLength; i++)
			{
				if (quote[i] < 65 || quote[i] > 90)
				{
					currquote[i] = quote[i];
				}
				else
				{
					currquote[i] = (quote[i] - 'A' + shift) % 26 + 'A';
				}
			}
			break;
		case (NORMALCIPHER): // vignere shift
			for (int i = 0; i < keyLength; i++)
			{
				key[i] = rand() % 26;
			}
			for (int i = 0; i <= quoteLength; i++)
			{
				if (quote[i] < 65 || quote[i] > 90)
				{
					currquote[i] = quote[i];
				}
				else
				{
					currquote[i] = (key[i % keyLength] + quote[i] - 'A') % 26 + 'A';
				}
			}
			break;
		case (INSANECIPHER):
			// random_shuffle(&alphabet[0], &alphabet[25]);
			// never mind idk how to use libraries Imma just implement Fisher-Yates algorithm
			for (int i = 25; i >= 0; i--)
			{
				int j = rand() % (i + 1);
				int temp = alphabet[i];
				alphabet[i] = alphabet[j];
				alphabet[j] = temp;
			}
			for (int i = 0; i <= quoteLength; i++)
			{
				if (quote[i] < 65 || quote[i] > 90)
				{
					currquote[i] = quote[i];
				}
				else
				{
					currquote[i] = alphabet[quote[i] - 'A'] + 'A';
				}
			}
			break;
		}

		// the alphabet gets randomly reassigned - vignere cipher
		//[1,2,3...25,0] is equivalently a caesar with shift=1
		Serial.println();
		for (int i = 0; i < 26; i++)
		{
			Serial.print(alphabet[i]);
			Serial.print(" ");
		}
		Serial.println();
		sprintf(output, currquote);
		Serial.println(currquote);
		cipherinputIndex = 0;
		// tft.println(output);
		cipher_state = CIPHERSOLVING;
		setup_joystick();
		break;
	}
	case (CIPHERSOLVING):
	{
		int change = loop_joystick();
		// if (cipherbutton1_click)
		// {
		// 	cipherinputIndex = (cipherinputIndex - 1 + quoteLength) % quoteLength;
		// }

		// if (cipherbutton2_click)
		if (cipherbutton3_click)
		{
			cipherinputIndex = (cipherinputIndex + 1) % quoteLength;
		}

		// if (cipherbutton3_click)
		// {

		// 	switch (cipher_difficulty)
		// 	{
		// 	case (EASYCIPHER):
		// 	{
		// 		shift = (shift + 1) % 26;
		// 		for (int i = 0; i <= quoteLength; i++)
		// 		{
		// 			if (quote[i] < 65 || quote[i] > 90)
		// 			{
		// 				currquote[i] = quote[i];
		// 			}
		// 			else
		// 			{
		// 				currquote[i] = (quote[i] - 'A' + shift) % 26 + 'A';
		// 			}
		// 		}
		// 		break;
		// 	}
		// 	case (NORMALCIPHER):
		// 	{
		// 		key[cipherinputIndex % keyLength] = (key[cipherinputIndex % keyLength] + 1) % 26;
		// 		for (int i = 0; i <= quoteLength; i++)
		// 		{
		// 			if (quote[i] < 65 || quote[i] > 90)
		// 			{
		// 				currquote[i] = quote[i];
		// 			}
		// 			else
		// 			{
		// 				currquote[i] = (key[i % keyLength] + quote[i] - 'A') % 26 + 'A';
		// 			}
		// 		}
		// 		Serial.println(quote);
		// 		Serial.println(currquote);
		// 		break;
		// 	}
		// 	case (INSANECIPHER):
		// 	{
		// 		char temp = quote[cipherinputIndex];
		// 		if (temp >= 65 && temp <= 90)
		// 		{
		// 			alphabet[temp - 65] = (alphabet[temp - 65] + 1) % 26;
		// 		}
		// 		for (int i = 0; i <= quoteLength; i++)
		// 		{
		// 			if (quote[i] < 65 || quote[i] > 90)
		// 			{
		// 				currquote[i] = quote[i];
		// 			}
		// 			else
		// 			{
		// 				currquote[i] = alphabet[quote[i] - 'A'] + 'A';
		// 			}
		// 		}
		// 		Serial.println(quote);
		// 		Serial.println(currquote);
		// 	}
		// 	}
		// }

		if (change == 1 && letters[0] >= 'A' && letters[0] <= 'Z' && letters[1] == '\0' && currquote[cipherinputIndex] >= 'A' && currquote[cipherinputIndex] <= 'Z')
		{
			switch (cipher_difficulty)
			{
			case (EASYCIPHER):
			{
				shift = (letters[0] - quote[cipherinputIndex]+26)%26;

				for (int i = 0; i <= quoteLength; i++)
				{
					if (quote[i] < 65 || quote[i] > 90)
					{
						currquote[i] = quote[i];
					}
					else
					{
						currquote[i] = (quote[i] - 'A' + shift) % 26 + 'A';
					}
				}
				break;
			}
			case (NORMALCIPHER):
			{
				key[cipherinputIndex % keyLength] = (letters[0] - quote[cipherinputIndex]+26)%26;
				for (int i = 0; i <= quoteLength; i++)
				{
					if (quote[i] < 65 || quote[i] > 90)
					{
						currquote[i] = quote[i];
					}
					else
					{
						currquote[i] = (key[i % keyLength] + quote[i] - 'A') % 26 + 'A';
					}
				}
				Serial.println(quote);
				Serial.println(currquote);
				break;
			}
			case (INSANECIPHER):
			{
				char temp = quote[cipherinputIndex];
				if (temp >= 65 && temp <= 90)
				{
					alphabet[temp - 65] = letters[0] - 'A';
				}
				for (int i = 0; i <= quoteLength; i++)
				{
					if (quote[i] < 65 || quote[i] > 90)
					{
						currquote[i] = quote[i];
					}
					else
					{
						currquote[i] = alphabet[quote[i] - 'A'] + 'A';
					}
				}
				Serial.println(quote);
				Serial.println(currquote);
			}
			}
		}

		char blink[quoteLength];
		for (int i = 0; i <= quoteLength; i++)
		{
			if (i == cipherinputIndex)
			{
				blink[i] = ' ';
			}
			else
			{
				blink[i] = currquote[i];
			}
		}
		if (strcmp(quote, currquote) == 0)
		{
			cipher_state = CIPHEROFF;
			sprintf(output, "%s                                                                                  ", currquote);
			tft.println(output);
			int cipherTimer = millis();
			while (millis() - cipherTimer < 2000)
				;
			tft.fillScreen(TFT_BLACK);
			mode = cipher_difficulty;
		}
		else
		{
			sprintf(output, currquote);
		}

		if (millis() % 1000 < 500)
			sprintf(output, "%s             ", currquote);
		else
			sprintf(output, "%s             ", blink);
		// tft.println(output);
		if (strcmp(prompt, output) != 0)
		{
			tft.setCursor(0, 0, 1);
			tft.println(output);
		}
		sprintf(prompt, output);
		break;
	}
	}
  Serial.println(letters);
	return mode;
}

void updateCipherDifficulty()
{
	cipher_difficulty = (cipher_difficulty + 1) % 3;
}

int get_quote(char *quote)
{
	char request_buffer[500];
	sprintf(request_buffer, "GET /random?minLength=80&maxLength=100 HTTP/1.1\r\n");
	strcat(request_buffer, "Host: api.quotable.io\r\n");
	strcat(request_buffer, "\r\n"); // new line from header to body
	Serial.println(request_buffer);
	do_http_request("api.quotable.io", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
	Serial.println("-----------");
	Serial.println(response);
	Serial.println("-----------");
	char *start = strchr(response, '{');
	char *end = strrchr(response, '}');
	DynamicJsonDocument doc(1024);
	deserializeJson(doc, start, end - start + 1);
	sprintf(quote, doc["content"]);
	return doc["length"];
}