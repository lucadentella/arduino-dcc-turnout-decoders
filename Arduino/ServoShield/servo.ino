// Move servo to the target position with the configured speed
void moveServoToTargetPosition(int outIndex, bool moveToPositionA) {
  
  // If servo is already moving, return
  if(servoOutputs[outIndex].moving) return;

  // get the number of steps to move from currentPosition to targetPosition
  int steps;
  byte currentPosition, targetPosition, speed;

  currentPosition = servoOutputs[outIndex].currentPosition;
  speed = servoOutputs[outIndex].speed;
  if(moveToPositionA) targetPosition = servoOutputs[outIndex].positionA;
  else targetPosition = servoOutputs[outIndex].positionB;
  
  if(targetPosition > currentPosition) {
    servoOutputs[outIndex].directionForward = true;
    steps = targetPosition - currentPosition;
  } else {
    servoOutputs[outIndex].directionForward = false;
    steps = currentPosition - targetPosition;    
  }

  if(steps > 0) {

    // get the delay between each step to match the speed, in milliseconds
    servoOutputs[outIndex].stepDelay = (speed * 100) / steps;

    // start the movement (controlled in the main loop())
    servoOutputs[outIndex].targetPosition = targetPosition;
    servoOutputs[outIndex].lastStepTime = millis();
    servoOutputs[outIndex].moving = true;

    char buf[100];
    sprintf(buf, "Moving servo from position %d to position %d with %d steps every %dms", currentPosition, targetPosition, steps, servoOutputs[outIndex].stepDelay);
    Serial.println(buf);
  }
}

// In programming mode, move servo to the given value
void moveServoToValue(int value) {
  
  pwmServo.writeMicroseconds(outIndex, value * usIncrementPerStep + SERVO_MIN_US);
}

// In the loop, move servos if needed
void moveServos() {

  // Get current time
  long currentTime = millis();

  // Loop for each output
  for(int i = 0; i < 16; i++) {

    // Servo is moving
    if(servoOutputs[i].moving) {

      // If the stepDelay has passed, do a step
      if(currentTime - servoOutputs[i].lastStepTime > servoOutputs[i].stepDelay) {
      
        if(servoOutputs[i].directionForward) servoOutputs[i].currentPosition = servoOutputs[i].currentPosition + 1;
        else servoOutputs[i].currentPosition = servoOutputs[i].currentPosition - 1;
        pwmServo.writeMicroseconds(i, servoOutputs[i].currentPosition * usIncrementPerStep + SERVO_MIN_US);
        servoOutputs[i].lastStepTime = currentTime;
        
        // If servo has reached the target position, stop
        if(servoOutputs[i].currentPosition == servoOutputs[i].targetPosition) {
          servoOutputs[i].moving = false;
          Serial.println("Target position reached");
          if(servoOutputs[i].targetPosition == servoOutputs[i].positionA) Dcc.setCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + LAST_POSITION_OFFSET, 0);
          else Dcc.setCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + LAST_POSITION_OFFSET, 1);
        }
      }
    }
  }
}

// If configuring the speed, sweep servo at the given speed
void sweepServo() {

  if(!sweepOutput) return;

  long currentTime = millis();

  // If the stepDelay has passed, do a step for the sweep output
  if(currentTime - servoOutputs[outIndex].lastStepTime > servoOutputs[outIndex].stepDelay) {

    if(servoOutputs[outIndex].directionForward) servoOutputs[outIndex].currentPosition = servoOutputs[outIndex].currentPosition + 1;
    else servoOutputs[outIndex].currentPosition = servoOutputs[outIndex].currentPosition - 1;
    pwmServo.writeMicroseconds(outIndex, servoOutputs[outIndex].currentPosition * usIncrementPerStep + SERVO_MIN_US);
    servoOutputs[outIndex].lastStepTime = currentTime;

    // If servo has reached the target position, reverse the movement
    if(servoOutputs[outIndex].currentPosition == servoOutputs[outIndex].targetPosition) toggleSweep();
  }
}

// Begin sweep movement
void beginSweep(bool fromAtoB) {

  if(fromAtoB) {
    servoOutputs[outIndex].currentPosition = servoOutputs[outIndex].positionA;
    servoOutputs[outIndex].targetPosition = servoOutputs[outIndex].positionB;
  } else {
    servoOutputs[outIndex].currentPosition = servoOutputs[outIndex].positionB;
    servoOutputs[outIndex].targetPosition = servoOutputs[outIndex].positionA;
  }

  // if sweep was not active, move servo to the start position
  if(!sweepOutput) {
    moveServoToValue(servoOutputs[outIndex].currentPosition);
    sweepOutput = true;
  }

  // setup variables
  if(servoOutputs[outIndex].targetPosition > servoOutputs[outIndex].currentPosition) {
    servoOutputs[outIndex].directionForward = true;
    sweepSteps = servoOutputs[outIndex].targetPosition - servoOutputs[outIndex].currentPosition;    
  } else {
    servoOutputs[outIndex].directionForward = false;
    sweepSteps = servoOutputs[outIndex].currentPosition - servoOutputs[outIndex].targetPosition;    
  }
  servoOutputs[outIndex].stepDelay = (servoOutputs[outIndex].speed * 100) / sweepSteps;    
}

// Change sweep direction
void toggleSweep() {

  if(servoOutputs[outIndex].targetPosition == servoOutputs[outIndex].positionA) beginSweep(true);
  else beginSweep(false);
}