// DCC Decoder for servo, 1 output
// Luca Dentella
//
// Playlist of my tutorials: https://youtube.com/playlist?list=PL45uNmSXju7mn3bMtVVPlFQmpD3mUI5mi

#include <NmraDcc.h>
#include <Servo.h>

// PIN definitions
#define DCC_PIN   2
#define ACK_PIN   3
#define RESET_PIN 4
#define SERVO_PIN 8

// Custom CVs
// Position value is 1000 + (CV_VALUE * 4)uS, for example for position = 2000uS set CV=250
// Speed is a multiple of 100milliseconds, for example for 2 seconds set CV=20
#define CV_A_POSITION     30
#define CV_B_POSITION     31
#define CV_SPEED          32
#define CV_LAST_POSITION  33

// Default values for CVs
#define DECODER_ADDR          4
#define DEFAULT_A_POSITION    20
#define DEFAULT_B_POSITION    200
#define DEFAULT_SPEED         30
#define DEFAULT_LAST_POSITION 0

// Global variables
NmraDcc Dcc;
Servo myServo;
byte aPosition, bPosition, speed;
byte currentPosition, targetPosition;
int stepDelay;
long lastStepTime = 0;
bool servoMoving = false;
bool directionForward = true;
char serialBuffer[100];

// Move servo to the target position with the configured speed
void moveServoToTargetPosition(byte position) {
  
  // get the number of steps to move from currentPosition to targetPosition
  targetPosition = position;
  byte steps;

  if(targetPosition > currentPosition) {
    directionForward = true;
    steps = targetPosition - currentPosition;
  } else {
    directionForward = false;
    steps = currentPosition - targetPosition;    
  }

  if(steps > 0) {

    // get the delay between each step to match the speed, in milliseconds
    stepDelay = (speed * 100) / steps;

    // start the movement (controlled in the main loop())
    lastStepTime = millis();
    servoMoving = true;

    char buf[100];
    sprintf(buf, "Moving servo from position %d to position %d with %d steps every %dms", currentPosition, targetPosition, steps, stepDelay);
    Serial.println(buf);
  }
}

// DCC acccessory decoder command received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {

  // Command sent to our address?
  if(Addr == Dcc.getAddr()) {

    // If servo is already moving, ignore the command
    if(!servoMoving) {
      if(Direction == 0) moveServoToTargetPosition(aPosition);
      else moveServoToTargetPosition(bPosition);
    } 
  }
}

void notifyCVAck(void) {

  digitalWrite(ACK_PIN, HIGH);
  delay(8);  
  digitalWrite(ACK_PIN, LOW);
}

// Factory reset (write to CV8)
void notifyCVResetFactoryDefault() {

  // Reset CVs to the default value
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DECODER_ADDR & 0xFF);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DECODER_ADDR >> 8);
  Dcc.setCV(CV_A_POSITION, DEFAULT_A_POSITION);
  Dcc.setCV(CV_B_POSITION, DEFAULT_B_POSITION);
  Dcc.setCV(CV_SPEED, DEFAULT_SPEED);
  Dcc.setCV(CV_LAST_POSITION, DEFAULT_LAST_POSITION);

  // Reset global variables
  aPosition = DEFAULT_A_POSITION;
  bPosition = DEFAULT_B_POSITION;
  speed = DEFAULT_SPEED;

  Serial.println("Factory reset:");
  sprintf(serialBuffer, "Address = %d, A Position = %d, B Position = %d, Speed = %d", DECODER_ADDR, DEFAULT_A_POSITION, DEFAULT_B_POSITION, DEFAULT_SPEED);
  Serial.println(serialBuffer);
  Serial.println();
};

void setup() {

  Serial.begin(115200);
  Serial.println("Servo Decoder");
  Serial.println();
  
  myServo.attach(SERVO_PIN);
  Dcc.pin(digitalPinToInterrupt(DCC_PIN), DCC_PIN, false);
  Dcc.init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);

  // Check if reset pin is LOW
  pinMode(RESET_PIN, INPUT_PULLUP);
  if(digitalRead(RESET_PIN) == LOW) notifyCVResetFactoryDefault();

  // Read CVs into global variables
  aPosition = Dcc.getCV(CV_A_POSITION);
  bPosition = Dcc.getCV(CV_B_POSITION);
  speed = Dcc.getCV(CV_SPEED);
  Serial.println("Values read from CVs:");
  sprintf(serialBuffer, "Address = %d, A Position = %d, B Position = %d, Speed = %d", Dcc.getAddr(), aPosition, bPosition, speed);
  Serial.println(serialBuffer);
  Serial.println();  

  // Move the servo immediately to the last position
  if(Dcc.getCV(CV_LAST_POSITION) == 0) currentPosition = aPosition;
  else currentPosition = bPosition;
  myServo.writeMicroseconds(currentPosition * 4 + 1000);
  sprintf(serialBuffer, "Servo moved to the last saved position, %d", currentPosition);
  Serial.println(serialBuffer);
  Serial.println("Decoder ready!");
}

void loop() {

  Dcc.process();

  // Check if servo is moving
  if(servoMoving) {

    long currentTime = millis();

    // If the stepDelay has passed, do a step
    if(currentTime - lastStepTime > stepDelay) {

      if(directionForward) currentPosition = currentPosition + 1;
      else currentPosition = currentPosition - 1;
      myServo.writeMicroseconds(currentPosition * 4 + 1000);
      lastStepTime = currentTime;

      // If servo has reached the target position, stop
      if(currentPosition == targetPosition) {
        servoMoving = false;
        Serial.println("Target position reached");
        Dcc.setCV(CV_LAST_POSITION, targetPosition);
      }
    }
  }
}
