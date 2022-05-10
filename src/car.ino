int driveState;

float carTimer;


void setup_car() {
  driveState = 0;

  // Left side of car
  digitalWrite(18, LOW);
  digitalWrite(19, HIGH);

  // Right side of car
  digitalWrite(20, HIGH);
  digitalWrite(21, LOW);

  carTimer = millis();
}

// Generalize this so that any shape with some side length can be driven
void loop_car() {
  switch (driveState) {
    case 0:
      if (millis() - carTimer > 3000) {
		Serial.println("Turning");
        digitalWrite(18, HIGH);
        digitalWrite(19, LOW);
        driveState = 1;
        carTimer = millis();
      }
	  break;
    case 1:
      if (millis() - carTimer > 2900) {
		Serial.println("Straightening");
        digitalWrite(18, LOW);
        digitalWrite(19, HIGH);
        driveState = 0;
        carTimer = millis();
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