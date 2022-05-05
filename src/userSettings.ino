const int ENTER = 0;
const int PRINT_INSTRUCT = 1;
const int LEADERBOARD = 2;
const int BRIGHTNESS = 3;

int user_state = 0;

// return 0 if still in settings, else 1
int handle_user_settings(){
  if (user_state == 0){ // just entered page
    tft.setRotation(2); //adjust rotation
    tft.setCursor(0,0,1);
    tft.setTextSize(1.5);
    tft.fillScreen(TFT_BLACK);
    print_settings_info();
    user_state = 1;
  }
  else if (user_state == 1){
  if (button38.update() == 1)
  {
    switch_leaderboard(); 
    user_state = 0;
  }
  // exit page
  if (button39.update() == 1) { // exit settings
      user_state = 0;
      return 1;
    }
  }
  return 0;
}

void print_settings_info(){
  tft.setTextSize(1.5);
  tft.printf("On Leaderboard?: %s", on_leaderboard);
  tft.println("");
  tft.println("");
  tft.println("");
  if (strcmp(on_leaderboard, "True") == 0) {
    tft.println("Button 38: Leave LeaderBoard");
  }
  else{
    tft.println("Button 38: Join LeaderBoard");
  }
  tft.println("");
  tft.println("");
  tft.println("");
  tft.println("Button 39: Back to Clock");
  // if user presses button to change, if var == "True" set to "False" else now "True"
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
}



