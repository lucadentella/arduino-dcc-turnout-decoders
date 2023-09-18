#include "ServoShield.h"

void setup() {
  
  Serial.begin(115200);
  Serial.println(BANNER);
  Serial.println();

  // Configure PINs direction
  pinMode(PROGRAM_LED_PIN, OUTPUT);
  pinMode(BTN_PROGRAM_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_UP_PIN, INPUT_PULLUP);
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

  // Init display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("- SSD1306 allocation failed, KO!"));
    for(;;); 
  }
  Serial.println("- Display init completed");

  // Init PWMServo library
   pwmServo.begin();
   pwmServo.setPWMFreq(SERVO_FREQ);
   Serial.println("- PWMServo library init completed");

  // Read CVs into running variables and set defaults
  for(int i = 0; i < 16; i++) {
    
    servoOutputs[i].positionA = Dcc.getCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + POSITION_A_OFFSET);
    servoOutputs[i].positionB = Dcc.getCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + POSITION_B_OFFSET);
    servoOutputs[i].speed = Dcc.getCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + SPEED_OFFSET);
    
    // Set currentPosition and targetPosition based on the last position stored in CV
    if(Dcc.getCV((CV_BASE_ADDRESS + OUT_SLOT_SIZE * i) + LAST_POSITION_OFFSET) == 0) {
      servoOutputs[i].currentPosition = servoOutputs[i].positionA;
      servoOutputs[i].targetPosition = servoOutputs[i].positionA;
    }
    else {
      servoOutputs[i].currentPosition = servoOutputs[i].positionB;
      servoOutputs[i].targetPosition = servoOutputs[i].positionB;
    }
    
    // Set default values
    servoOutputs[i].stepDelay = 0;
    servoOutputs[i].lastStepTime = 0;
    servoOutputs[i].moving = false;
    servoOutputs[i].directionForward = true;
  }
  Serial.println("- CVs loaded and variables initialized");
  Serial.println();  

  // Program mode is disabled at start
  toggleProgramMode();   
}

void loop() {

  // Process DCC commands and move servos if NOT in program mode
  if(!inProgramMode) {

    // process DCC messages
    Dcc.process();

    // move servos
    moveServos();
  } 
  
  // If in program mode, sweep servo if configuring speed
  else sweepServo();

  // Always check buttons
  checkButtons();
}
