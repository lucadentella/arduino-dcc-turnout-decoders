void toggleProgramMode() {

  // Enter program mode
  if(!inProgramMode) {
    inProgramMode = true;
    programmingAddress = true;
    newAddressValue = Dcc.getAddr();
    digitalWrite(PROGRAM_LED_PIN, HIGH);
    updateFullDisplayAddress();
  }

  // Exit program mode
  else {
    inProgramMode = false;
    sweepOutput = false;
    digitalWrite(PROGRAM_LED_PIN, LOW);
    display.clearDisplay();
    display.display();
  }
}

void changeCurrentValue(int increment) {

  // address programming
  if(programmingAddress) {

    // increment the address and display the new value
    newAddressValue += increment;
    updateDisplayValueAddress();
  }

  // servo programming
  else {

    // increment the corrent value, move the servo (or change its speed) and display the new value
    int newValue = getCurrentValue() + increment;
    if(newValue >= 0 && newValue < 256) { 
      storeValue(newValue);
      if(outSubIndex == POSITION_A_OFFSET || outSubIndex == POSITION_B_OFFSET) moveServoToValue(newValue);
      else if(outSubIndex == SPEED_OFFSET) servoOutputs[outIndex].stepDelay = (newValue * 100) / sweepSteps;
      updateDisplayValue(newValue);
    }
  }
}

void changeConfigItem() {

  // address programming
  if(programmingAddress) {

    // store it in the corresponding CVs
    Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, newAddressValue & 0xFF);
    Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, newAddressValue >> 8);

    // enter servo programming
    programmingAddress = false;
    outIndex = 0;
    outSubIndex = 0;
    updateFullDisplayServo();
    moveServoToValue(getCurrentValue());
  }

  // servo programming
  else {

    // Store the new value in EEPROM
    Dcc.setCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * outIndex) + outSubIndex, getCurrentValue());

    // Move to the next subelement
    if(outSubIndex < 2) {
      outSubIndex++;
      updateFullDisplayServo();
      if(outSubIndex == POSITION_A_OFFSET || outSubIndex == POSITION_B_OFFSET) moveServoToValue(getCurrentValue());
      else if(outSubIndex == SPEED_OFFSET) beginSweep(true);
    } 

    // move to the next element
    else {

      if(outIndex < 15) {
        outIndex++;
        outSubIndex = 0;
        updateFullDisplayServo();
        sweepOutput = false;
        moveServoToValue(getCurrentValue());
      }

      // end of elements reached
      else {
        programmingAddress = true;
        updateFullDisplayAddress();
      }
    }
  }
}

byte getCurrentValue() {

  if(outSubIndex == POSITION_A_OFFSET) return servoOutputs[outIndex].positionA;
  else if(outSubIndex == POSITION_B_OFFSET) return servoOutputs[outIndex].positionB;
  else if(outSubIndex == SPEED_OFFSET) return servoOutputs[outIndex].speed;
}

void storeValue(int value) {

  if(outSubIndex == POSITION_A_OFFSET)servoOutputs[outIndex].positionA = value;
  else if(outSubIndex == POSITION_B_OFFSET) servoOutputs[outIndex].positionB = value;
  else if(outSubIndex == SPEED_OFFSET) servoOutputs[outIndex].speed = value;
}