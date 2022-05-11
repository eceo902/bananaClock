
float currentX = 12;
float currentY = 12;
float pastX=12;
float pastY=12;
float angle_thresh = 0.3;

//DIMENSIONS = 128 by 160
//tft.drawRect(xLeft, yUp, xRight (128), yDown(160), TFT_WHITE);
//size of cubes: 8 by 8

int mazeDesign[] = {

  1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 
  1, 0, 1, 0, 0,   0, 0, 0, 0, 0,   0, 0, 0, 1, 0,   0, 0, 1, 0, 1,
  1, 0, 1, 0, 0,   0, 1, 1, 1, 1,   1, 1, 0, 0, 0,   1, 0, 0, 0, 1,
  1, 0, 1, 1, 1,   0, 1, 0, 0, 0,   0, 0, 0, 1, 1,   1, 1, 1, 1, 1,

  1, 0, 0, 0, 1,   0, 1, 1, 1, 1,   1, 1, 0, 1, 0,   0, 0, 0, 0, 1,
  1, 0, 1, 0, 1,   0, 1, 0, 0, 0,   0, 1, 0, 1, 0,   1, 1, 1, 0, 1,
  1, 0, 1, 0, 1,   0, 1, 0, 1, 0,   0, 1, 0, 1, 0,   1, 0, 0, 0, 1,
  1, 0, 1, 0, 1,   0, 1, 0, 1, 1,   0, 1, 0, 1, 0,   1, 0, 1, 1, 1,

  1, 0, 1, 0, 0,   0, 1, 0, 0, 1,   0, 1, 0, 1, 0,   1, 0, 1, 0, 1,
  1, 0, 1, 1, 1,   1, 1, 1, 0, 1,   0, 1, 0, 1, 0,   1, 0, 1, 0, 1,
  1, 0, 0, 0, 0,   0, 0, 0, 0, 1,   0, 1, 0, 1, 0,   1, 0, 0, 0, 1,
  1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   0, 1, 0, 1, 0,   1, 1, 1, 0, 1,

  1, 0, 1, 0, 0,   0, 0, 0, 0, 0,   0, 1, 0, 1, 0,   1, 0, 0, 0, 1,
  1, 0, 1, 0, 1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 0,   1, 0, 1, 1, 1,
  1, 0, 0, 0, 0,   0, 0, 0, 0, 0,   0, 0, 0, 0, 0,   1, 0, 0, 2, 1,
  1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 1

};

void drawAtCord(int xInd, int yInd, int color){ //0 black, 1 white

  if (color == 1){
    tft.fillRect(xInd*8+4, yInd*8+4, 7, 7, TFT_WHITE);
  } else if (color == 2) {
    tft.fillRect(xInd*8+4, yInd*8+4, 7, 7, TFT_RED);
  } else {
    tft.fillRect(xInd*8+4, yInd*8+4, 7, 7, TFT_BLACK);
  }
}

void drawPlayer(int xInd, int yInd, int color){ //1 blue, 0 black
  if (color == 1){
    tft.fillRect(xInd, yInd, 4, 4, TFT_PINK);
  } else {
    tft.fillRect(xInd, yInd, 4, 4, TFT_BLACK);
  }
}

int coordinate(int xCord, int yCord){
  int xx = xCord/8;
  int yy = yCord/8;
  return xx*20 + yy;
}

void setupMaze(){
  tft.fillScreen(TFT_BLACK);
  //tft.drawRect(2, 20, 120, 90, TFT_WHITE);

  for (int xx =0; xx < 16; xx++){

    for (int yy = 0; yy < 20; yy++){
      drawAtCord(xx, yy, mazeDesign[xx*20 + yy]);
    }
  }
      drawPlayer(currentX, currentY, 1);
}

int loopMaze(){
  if (x > angle_thresh){
    int coor = coordinate(currentX, currentY+2);
    if (mazeDesign[coor] == 0 ){
      pastY = currentY;
      pastX = currentX;
      currentY += 0.1;
      drawPlayer(pastX, pastY, 0);
      drawPlayer(currentX, currentY, 1);
      Serial.println("x > angle");
    } else if (mazeDesign[coor] == 2){
      return 0;
    }
    
  } else if (x < - angle_thresh){
    int coor = coordinate(currentX, currentY-5);
    if (mazeDesign[coor] == 0 ){
      pastY = currentY;
      pastX = currentX;
      currentY -= 0.1;
      drawPlayer(pastX, pastY, 0);
      drawPlayer(currentX, currentY, 1);
      Serial.println("x < angle");
    } else if (mazeDesign[coor] == 2){
      return 0;
    }

  } else if (y > angle_thresh){
    int coor = coordinate(currentX+2, currentY);
    if (mazeDesign[coor] == 0 ){
      pastX = currentX;
      pastY = currentY;
      currentX += 0.1;
      drawPlayer(pastX, pastY, 0);
      drawPlayer(currentX, currentY, 1);
      Serial.println("Y > angle");
    } else if (mazeDesign[coor] == 2){
      return 0;
    }
  } else if (y < - angle_thresh){
    int coor = coordinate(currentX-5, currentY);
    if (mazeDesign[coor] == 0 ){
      pastX = currentX;
      pastY = currentY;
      currentX -= 0.1;
      drawPlayer(pastX, pastY, 0);
      drawPlayer(currentX, currentY, 1);
      Serial.println("Y < angle");
    } else if (mazeDesign[coor] == 2){
      return 0;      
    }
  }
  return -1;
}