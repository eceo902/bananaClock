#define BACKGROUND TFT_BLACK
#define BALL_COLOR TFT_BLUE

const int DT = 10; //milliseconds

struct Vec { //C struct to represent 2D vector (position, accel, vel, etc)
  float x;
  float y;
};

uint32_t primary_timer; //main loop timer

//state variables:
struct Vec position; //position of ball
struct Vec velocity; //velocity of ball

const int RADIUS = 3; //radius of ball

//boundary constants:
const int LEFT_LIMIT = RADIUS; //left side of screen limit
const int RIGHT_LIMIT = 127 - RADIUS; //right side of screen limit
const int TOP_LIMIT = RADIUS; //top of screen limit
const int BOTTOM_LIMIT = 159 - RADIUS; //bottom of screen limit

const int vertOffset = 110;
const int qOffset = 5;
const int aOffset = 13;
const int zOffset = 18;


int i;


void step(float upDownVal, float leftRightVal) {
  velocity.x = 0;
  if (upDownVal > 3200) velocity.x = -1;
  else if (upDownVal < 1500) velocity.x = 1;
  velocity.y = 0;
  if (leftRightVal > 3200) velocity.y = -1;
  else if (leftRightVal < 1500) velocity.y = 1;
  moveBall();
}

void moveBall() {
  float new_x = position.x + velocity.x;
  if (new_x < LEFT_LIMIT){
    position.x = LEFT_LIMIT;
  }
  else if (new_x > RIGHT_LIMIT){
    position.x = RIGHT_LIMIT;
  }
  else position.x = new_x;
  
  float new_y = position.y + velocity.y;
  if (new_y < TOP_LIMIT){
    position.y = TOP_LIMIT;
  }
  else if (new_y > BOTTOM_LIMIT){
    position.y = BOTTOM_LIMIT;
  }
  else position.y = new_y;
}




void setup_joystick() {
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(BACKGROUND);

  i = 0;
  tft.fillCircle(position.x, position.y, RADIUS, BALL_COLOR); //draw new ball location
  
  tft.setCursor(qOffset, vertOffset, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Q W E R T Y U I O P\n");
  tft.println("  A S D F G H J K L\n");
  tft.println("   Z X C V B N M");

  tft.setCursor(0, 0, 1);
  tft.println(prompt);
  
  primary_timer = millis();
}

int loop_joystick() {
  int upDown = analogRead(1);
  Serial.println(upDown);
  int leftRight = analogRead(2);
  Serial.println(leftRight);

  int typeInput = button45.update();
  if (typeInput != 0 && position.y > vertOffset - 5) {    // Weird but we need to check if we are past the vertical offset in this if-statement
    if (position.y < vertOffset + 11 && position.x > qOffset - 3) {
           if (position.x < qOffset + 8) letters[strlen(letters)] = 'Q';
      else if (position.x < (qOffset + 8) + (1 * 12)) letters[strlen(letters)] = 'W';
      else if (position.x < (qOffset + 8) + (2 * 12)) letters[strlen(letters)] = 'E';
      else if (position.x < (qOffset + 8) + (3 * 12)) letters[strlen(letters)] = 'R';
      else if (position.x < (qOffset + 8) + (4 * 12)) letters[strlen(letters)] = 'T';
      else if (position.x < (qOffset + 8) + (5 * 12)) letters[strlen(letters)] = 'Y';
      else if (position.x < (qOffset + 8) + (6 * 12)) letters[strlen(letters)] = 'U';
      else if (position.x < (qOffset + 8) + (7 * 12)) letters[strlen(letters)] = 'I';
      else if (position.x < (qOffset + 8) + (8 * 12)) letters[strlen(letters)] = 'O';
      else if (position.x < (qOffset + 8) + (9 * 12)) letters[strlen(letters)] = 'P';
    }
    else if (position.y < vertOffset + 27 && position.x > aOffset - 3) {
           if (position.x < aOffset + 8) letters[strlen(letters)] = 'A';
      else if (position.x < (aOffset + 8) + (1 * 12)) letters[strlen(letters)] = 'S';
      else if (position.x < (aOffset + 8) + (2 * 12)) letters[strlen(letters)] = 'D';
      else if (position.x < (aOffset + 8) + (3 * 12)) letters[strlen(letters)] = 'F';
      else if (position.x < (aOffset + 8) + (4 * 12)) letters[strlen(letters)] = 'G';
      else if (position.x < (aOffset + 8) + (5 * 12)) letters[strlen(letters)] = 'H';
      else if (position.x < (aOffset + 8) + (6 * 12)) letters[strlen(letters)] = 'J';
      else if (position.x < (aOffset + 8) + (7 * 12)) letters[strlen(letters)] = 'K';
      else if (position.x < (aOffset + 8) + (8 * 12)) letters[strlen(letters)] = 'L';
    }
    else if (position.y < vertOffset + 43 && position.x > zOffset - 3) {
           if (position.x < zOffset + 8) letters[strlen(letters)] = 'Z';
      else if (position.x < (zOffset + 8) + (1 * 12)) letters[strlen(letters)] = 'X';
      else if (position.x < (zOffset + 8) + (2 * 12)) letters[strlen(letters)] = 'C';
      else if (position.x < (zOffset + 8) + (3 * 12)) letters[strlen(letters)] = 'V';
      else if (position.x < (zOffset + 8) + (4 * 12)) letters[strlen(letters)] = 'B';
      else if (position.x < (zOffset + 8) + (5 * 12)) letters[strlen(letters)] = 'N';
      else if (position.x < (zOffset + 8) + (6 * 12)) letters[strlen(letters)] = 'M';
    }
    tft.setCursor(0, 90, 1);
    tft.println(letters);
  }

  int deleteInput = button39.update();
  if (deleteInput != 0 && strlen(letters) > 0) {
    letters[strlen(letters) - 1] = '\0';
    tft.setCursor(0, 90, 1);
    tft.println("                                       ");
    tft.setCursor(0, 90, 1);
    tft.println(letters);
  }
  
  int prevX = position.x;
  int prevY = position.y;
  step(upDown, leftRight);
  if (position.x != prevX || position.y != prevY) {  
    i += 1;
    // only redraw keyboard, prompt, and word every 5 cursor moves
    if (i == 5) {
      tft.setCursor(qOffset, vertOffset, 1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.println("Q W E R T Y U I O P\n");
      tft.println("  A S D F G H J K L\n");
      tft.println("   Z X C V B N M");

      tft.setCursor(0, 0, 1);
      tft.println(prompt);

      tft.setCursor(0, 90, 1);
      tft.println(letters);

      i = 0;
    }
    
    //draw circle in previous location of ball in color background (redraws minimal num of pixels, therefore is quick!)
    tft.fillCircle(prevX, prevY, RADIUS, BACKGROUND);
    tft.fillCircle(position.x, position.y, RADIUS, BALL_COLOR); //draw new ball location
  }

  while (millis() - primary_timer < DT); //wait for primary timer to increment
  primary_timer = millis();
  
  int submitInput = button38.update();
  if (submitInput != 0) {
    return 1;
  }
  return 0;
}