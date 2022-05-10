const int ENTER = 0;
const int PRINT_INSTRUCT = 1;
const int LEADERBOARD = 2;
const int BRIGHTNESS = 3;

int user_state = 0;

// return 0 if still in settings, else 1
int handle_user_settings(int update_39, int update_38){
  // int update_39 = button39.update();
  // int update_38 = button38.update();
  
  if (user_state == 0){ // just entered page
    tft.setRotation(2); //adjust rotation
    tft.setCursor(0,0,1);
    tft.setTextSize(1.5);
    tft.fillScreen(TFT_BLACK);
    print_settings_info1();
    user_state = 1;
  }
  else if (user_state == 1){
    if (update_39 != 0)
    {
      switch_leaderboard(); 
    }
    else if (update_38 != 0) 
    { // Confirm
      user_state = 2;
      print_settings_info2();
    }
  }
  else if (user_state == 2){
    if (update_39 != 0)
    {
      Serial.println(ambientAmt);
      if (ambientAmt >= 4095.0){
        ambientAmt = 0.0;
      }
      else{
        ambientAmt += 4095/10;
      }
    }
    else if (update_38 != 0) { // exit settings
      user_state = 0;
      return 1;
    }
  }
  return 0;
}

void print_settings_info1(){
  tft.setCursor(0,0,1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1.5);
  tft.printf("On Leaderboard?: %s", on_leaderboard);
  tft.println("");
  tft.println("");
  tft.println("");
  if (strcmp(on_leaderboard, "True") == 0) {
    tft.println("Button 39: Leave LeaderBoard");
  }
  else{
    tft.println("Button 39: Join LeaderBoard");
  }
  tft.println("");
  tft.println("");
  tft.println("");
  tft.println("Button 38: Confirm Setting");
}

void print_settings_info2(){
  tft.setCursor(0,0,1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1.5);
  tft.println("Button 39: Cycle Brightness");
  tft.println("");
  tft.println("");
  tft.println("");
  tft.println("Button 38: Confirm Setting");
}

void switch_leaderboard(){
    // if user presses button to change, if var == "True" set to "False" else now "True"
    if (strcmp(on_leaderboard, "True") == 0) {
      on_leaderboard[0] = '\0';
      sprintf(on_leaderboard, "%s", "False");
    }
    else{
      on_leaderboard[0] = '\0';
      sprintf(on_leaderboard, "%s", "True");
    }
    print_settings_info1();      
}



