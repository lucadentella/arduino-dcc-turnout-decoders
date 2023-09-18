// DCC acccessory decoder command received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {

  // Is command for one of the 16 outputs?
  uint16_t myAddress = Dcc.getAddr();
  if(Addr >= myAddress && Addr < myAddress + 16) {

    int output = Addr - myAddress;
    if(Direction == 0) moveServoToTargetPosition(output, true);
    else moveServoToTargetPosition(output, false);
  }
}

// Send ACK for reading CVs
void notifyCVAck(void) {

  digitalWrite(ACK_PIN, HIGH);
  delay(10);  
  digitalWrite(ACK_PIN, LOW);
}

// Factory reset (write to CV8)
void notifyCVResetFactoryDefault() {

  Serial.println("Factory reset, set all CVs to default values");

  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DEFAULT_DECODER_ADDRESS & 0xFF);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DEFAULT_DECODER_ADDRESS >> 8);
  for(int i = 0; i < 16; i++) {
    Dcc.setCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + POSITION_A_OFFSET, DEFAULT_POSITION_A);  
    Dcc.setCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + POSITION_B_OFFSET, DEFAULT_POSITION_B);
    Dcc.setCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + SPEED_OFFSET, DEFAULT_SPEED);
  }

  for(int i = 0; i < 5; i++) {
    digitalWrite(PROGRAM_LED_PIN, HIGH);
    delay(100);
    digitalWrite(PROGRAM_LED_PIN, LOW);
    delay(100);
  }
};