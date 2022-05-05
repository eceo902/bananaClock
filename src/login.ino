void loop_setup() {
	
}

int loop_login() {
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