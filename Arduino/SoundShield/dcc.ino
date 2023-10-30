// DCC acccessory decoder command received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {

  // in programming mode, change the decoder address based on the one received
  if(inProgrammingMode) {

    Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, Addr & 0xFF);
    Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, Addr >> 8);
    Serial.print("New decoder address: ");
    Serial.println(Dcc.getAddr());

    // confirm new address blinking the LED and exit programming mode
    blinkProgramLED(3);
    inProgrammingMode = false;
    return;
  }

  // Is command for one of available files?
  uint16_t myAddress = Dcc.getAddr();
  if(Addr >= myAddress && Addr < myAddress + fileCount) {

    // Track number starts at 1
    int cvIndex = Addr - myAddress;
    int fileIndex = cvIndex + 1;


    // get play mode and volume for the specific file
    byte filePlayMode = Dcc.getCV(CV_BASE_ADDRESS + 2 * cvIndex);
    byte fileVolume = Dcc.getCV(CV_BASE_ADDRESS + 2 * cvIndex + 1);

    // in SINGLE MODE, play file
    if(filePlayMode == PLAY_SINGLE) {
      Serial.print("Playing in SINGLE file number ");
      Serial.println(fileIndex);
      mp3.setLoopMode(MP3_LOOP_NONE);
      mp3.setVolume(fileVolume);
      mp3.playFileByIndexNumber(fileIndex);      
    }

    // in LOOP MODE direction starts or stops playing
    if(filePlayMode == PLAY_LOOP)
      
      // PLAY
      if(Direction == 0) {
        Serial.print("Playing in LOOP file number ");
        Serial.println(fileIndex);
        mp3.setLoopMode(MP3_LOOP_ONE);
        mp3.setVolume(fileVolume);
        mp3.playFileByIndexNumber(fileIndex);
      }

      // STOP
      else {
        Serial.println("Stop playing");
        mp3.pause();
      }
  }
}

// Send ACK for reading CVs
void notifyCVAck(void) {

  digitalWrite(ACK_PIN, HIGH);
  delay(12);  
  digitalWrite(ACK_PIN, LOW);
}

// Factory reset (write to CV8)
void notifyCVResetFactoryDefault() {

  Serial.println("Factory reset, set all CVs to default values");

  // Address
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DEFAULT_DECODER_ADDRESS & 0xFF);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DEFAULT_DECODER_ADDRESS >> 8);
  
  // CVs for DecoderPro
  Dcc.setCV(47, CV47_COUNTRY_CODE);
  Dcc.setCV(48, CV48_DEVELOPER_CODE);
  Dcc.setCV(49, CV49_THIRD_PRODUCT_ID);
  Dcc.setCV(50, CV50_FOURTH_PRODUCT_ID);

  // Tracks  
  for(int i = CV_BASE_ADDRESS; i < 1024; i = i + 2) {
    Dcc.setCV(i, DEFAULT_PLAY_MODE);  
    Dcc.setCV(i + 1, DEFAULT_VOLUME);
  }
  Serial.println("Done!");
  Serial.println();

  blinkProgramLED(5);
};

void blinkProgramLED(int num) {

    for(int i = 0; i < num; i++) {
    digitalWrite(PROGRAM_LED_PIN, HIGH);
    delay(100);
    digitalWrite(PROGRAM_LED_PIN, LOW);
    delay(100);
  }
}