int driveState;

float timer;


void setup_car() {
  driveState = 0;

  timer = millis();
}

// Generalize this so that any shape with some side length can be driven
void loop_car() {
  switch (driveState) {
    case 0:
      if (millis() - timer > 3000) {
        digitalWrite(18, LOW);
        digitalWrite(19, HIGH);
        driveState = 1;
        timer = millis();
      }
    case 1:
      if (millis() - timer > 2900) {
        digitalWrite(18, HIGH);
        digitalWrite(19, LOW);
        driveState = 0;
        timer = millis();
      }
  }
}

void stop_car() {
  // Left side of car
  digitalWrite(18, LOW);
  digitalWrite(19, LOW);

  // Right side of car
  digitalWrite(20, LOW);
  digitalWrite(21, LOW);
}