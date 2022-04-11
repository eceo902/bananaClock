#include "Arduino.h"
#include "Button.h"

//enum for button states
Button::Button(int p) {
  flag = 0;  
  state = 0;
  pin = p;
  S2_start_time = millis(); //init
  button_change_time = millis(); //init
  debounce_duration = 10;
  long_press_duration = 1000;
  button_pressed = 0;
}
void Button::read() {
  uint8_t button_val = digitalRead(pin);  
  button_pressed = !button_val; //invert button
}
int Button::update() {
  read();
  flag = 0;
  if (state==0) {
    if (button_pressed) {
      state = 1;
      button_change_time = millis();
    }
  } 
  else if (state==1) {
    if (millis() - button_change_time > debounce_duration) {
      state = 2;
      S2_start_time = millis();
    }
    else if (!button_pressed) {
      state = 0;
      button_change_time = millis();
    }
  } 
  else if (state==2) {
    if (millis() - S2_start_time > long_press_duration) {
      state = 3;
    }
    else if (!button_pressed) {
      state = 4;
      button_change_time = millis();
    }
  } 
  else if (state==3) {
    if (!button_pressed) {
      state = 4;
      button_change_time = millis();
    }
  } 
  else if (state==4) {      	
    if (millis() - button_change_time > debounce_duration) {
      if (millis() - S2_start_time > long_press_duration) {
        flag = 2;
      }
      else {
        flag = 1;
      }
      state = 0;
    }
    else if (button_pressed) {
      if (millis() - S2_start_time > long_press_duration) {
        state = 3;
      }
      else {
        state = 2;
      }
      button_change_time = millis();
    }
  }
  return flag;
}
