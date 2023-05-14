// Finite State Machine for programming sequence
#define PGM_SET_A_POSITION    0
#define PGM_SET_B_POSITION    1
#define PGM_SET_SPEED         2
int pgmState;

void checkProgrammingMode() {

  // 1. We're not in programming mode = enter programming mode if programButton stays down for at least 2 seconds
  if(!programmingMode) {

    int programButtonStatus = digitalRead(PROGRAM_PIN);

    // Button pressed
    if(programButtonStatus == LOW) {
      
      // is this the first time we see it pressed? Start counting the time...
      if(!isProgramButtonDown) {
        isProgramButtonDown = true;
        lastProgramButtonDown = millis();
      } 
      // two seconds have passsed? Enter programming mode to set A POSITION
      else {
        if(millis() - lastProgramButtonDown > 2000) {
          while(digitalRead(PROGRAM_PIN) == LOW);
          programmingMode = true;
          pgmState = PGM_SET_A_POSITION;
          myServo.writeMicroseconds(aPosition * 4 + 1000);
          digitalWrite(LED_PIN, HIGH);
          Serial.println("Enter programming mode");       
        }
      }
    } else isProgramButtonDown = false;
  }

  // 2. We're in programming mode, 3 different steps: SET A POSITION, SET B POSITION, SET SPEED
  else {

    switch(pgmState) {

      case PGM_SET_A_POSITION:

        // Increase Value
        if(digitalRead(UP_PIN) == LOW) {
          delay(50);
          if(digitalRead(UP_PIN) == LOW) {
            if(aPosition < 255) aPosition++;
            myServo.writeMicroseconds(aPosition * 4 + 1000);            
          }
        }
        // Decrease Value
        if(digitalRead(DOWN_PIN) == LOW) {
          delay(50);
          if(digitalRead(DOWN_PIN) == LOW) {
            if(aPosition > 0) aPosition--;
            myServo.writeMicroseconds(aPosition * 4 + 1000);
          }
        }
        // Store value and move to the next step
        if(digitalRead(PROGRAM_PIN) == LOW) {
          delay(50);
          if(digitalRead(PROGRAM_PIN) == LOW) {
            while(digitalRead(PROGRAM_PIN) == LOW);
            Dcc.setCV(CV_A_POSITION, aPosition);
            pgmState = PGM_SET_B_POSITION;
            myServo.writeMicroseconds(bPosition * 4 + 1000);
            Serial.print("New A Position = ");
            Serial.println(aPosition);
          }
        }                
        break;

      case PGM_SET_B_POSITION:

        // Increase Value
        if(digitalRead(UP_PIN) == LOW) {
          delay(50);
          if(digitalRead(UP_PIN) == LOW) {
            if(bPosition < 255) bPosition++;
            myServo.writeMicroseconds(bPosition * 4 + 1000);            
          }
        }
        // Decrease Value
        if(digitalRead(DOWN_PIN) == LOW) {
          delay(50);
          if(digitalRead(DOWN_PIN) == LOW) {
            if(bPosition > 0) bPosition--;
            myServo.writeMicroseconds(bPosition * 4 + 1000);
          }
        }
        // Store value and move to the next step
        if(digitalRead(PROGRAM_PIN) == LOW) {
          delay(50);
          if(digitalRead(PROGRAM_PIN) == LOW) {
            while(digitalRead(PROGRAM_PIN) == LOW);
            Dcc.setCV(CV_B_POSITION, bPosition);
            pgmState = PGM_SET_SPEED;
            servoSweep = true;
            myServo.writeMicroseconds(aPosition);
            AtoBdirection = true;
            targetPosition = bPosition;          
            Serial.print("New B Position = ");
            Serial.println(bPosition);            
          }
        }                
        break;

      case PGM_SET_SPEED:

        // Increase Value
        if(digitalRead(UP_PIN) == LOW) {
          delay(50);
          if(digitalRead(UP_PIN) == LOW) {
            if(speed < 255) speed++;
            stepDelay = (speed * 100) / steps;        
          }
        }
        // Decrease Value
        if(digitalRead(DOWN_PIN) == LOW) {
          delay(50);
          if(digitalRead(DOWN_PIN) == LOW) {
            if(speed > 0) speed--;
            stepDelay = (speed * 100) / steps;
          }
        }
        // Store value and exit programming
        if(digitalRead(PROGRAM_PIN) == LOW) {
          delay(50);
          if(digitalRead(PROGRAM_PIN) == LOW) {
            while(digitalRead(PROGRAM_PIN) == LOW);
            Dcc.setCV(CV_SPEED, speed);
            programmingMode = false;
            digitalWrite(LED_PIN, LOW);
            Serial.print("New Speed = ");
            Serial.println(speed);
            servoSweep = false;            
            Serial.println("Exit programming mode"); 
          }
        }                
        break;
    }

  }
}