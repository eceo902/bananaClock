class TimeGetter {
    int hour_index;
    int alphabet[62] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60};
    char msg[400] = {0}; //contains previous query response
    char query_string[50] = {0};
    int state;
    uint32_t scroll_timer;
    const int scroll_threshold = 500;
    const float angle_threshold = 0.35;

  public:
    TimeGetter() {
      state = 0;
      hour_index = 0;
      scroll_timer = millis();
    }
  void scroll_prints(int hour_index, int max_val){
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 1);
      if (max_val == 23)
      {
        tft.println("Button39");
        tft.println("Confirm Hour");
      }
      else if (max_val == 59){
        tft.println("Button39");
        tft.println("Confirm Minute");
      }
      tft.println("");
      tft.printf("Selected: %d", hour_index);
    }
    int update(float angle, int button, int max_val) 
    {
      tft.setTextSize(1.75);
      if (state == 0)
      {
        state = 1;
      }
      if (state == 1)
      { //time selection
        if((button==1) && (millis() - scroll_timer >= 100)) //PRESSED BUTTON TO SELECT GAME
        { 
          return hour_index;
        } 
        else if (scroll_threshold<=millis()-scroll_timer){
        if (angle_threshold <= angle)
        {
          scroll_timer = millis();
          scroll_prints(hour_index, max_val);
          if(hour_index>=max_val){
            hour_index=0;
          }else{
            hour_index+=1; 
          }
        } 
        else if(angle<=-angle_threshold)
        { 
          scroll_timer = millis();
          scroll_prints(hour_index, max_val);
          if(hour_index<=0){
              hour_index=max_val;
          }else{
              hour_index-=1;
          }  
        }
  }
  }
  return -1;
}
};
 