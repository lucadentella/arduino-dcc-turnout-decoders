#include "SoundShield.h"

void setup() {
  
  Serial.begin(115200);
  Serial.println(BANNER);
  Serial.println();

  // Configure PINs direction
  pinMode(ACK_PIN, OUTPUT);
  pinMode(PROGRAM_LED_PIN, OUTPUT);
  pinMode(BTN_PROGRAM_PIN, INPUT_PULLUP);
  Serial.println("- PINs configuration completed");

  // Check if factory reset is required
  if(digitalRead(BTN_PROGRAM_PIN) == LOW) {
    delay(100);
    if(digitalRead(BTN_PROGRAM_PIN) == LOW) notifyCVResetFactoryDefault();
  }

  // Init DCC library
  Dcc.pin(digitalPinToInterrupt(DCC_PIN), DCC_PIN, false);
  Dcc.init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);
  Serial.println("- DCC library init completed");

  // Configure SoftwareSerial for mp3 module communication
  mySerial.begin(9600);
  Serial.println("- SoftwareSerial configuration completed");  

  // Reset the module and count the available files
  mp3.reset();
  fileCount = mp3.countFiles(MP3_SRC_BUILTIN);
  Serial.print("- MP3 module reset completed, found ");
  Serial.print(fileCount);
  Serial.println(" files");

  Serial.println();  
}

void loop() {

  // process DCC messages
  Dcc.process();

  // check programming button
  if(digitalRead(BTN_PROGRAM_PIN) == 0) {

    // was button already down?
    if(!btnProgramDown) {
    
      // set status and debounce
      btnProgramDown = true;
      delay(50);

      // enter or exit programming mode
      if(digitalRead(BTN_PROGRAM_PIN) == 0) {
        if(inProgrammingMode) {
          Serial.println("Exit programming mode");
          inProgrammingMode = false;
          digitalWrite(PROGRAM_LED_PIN, LOW);
        } else {
          Serial.println("Enter programming mode");
          inProgrammingMode = true;
          digitalWrite(PROGRAM_LED_PIN, HIGH);        
        }
      } 
    } 
  } else btnProgramDown = false;
}
