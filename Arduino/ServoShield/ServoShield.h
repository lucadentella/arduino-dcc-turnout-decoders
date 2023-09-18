// Libraries
#include <SPI.h>
#include <Wire.h>
#include <NmraDcc.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>
#include <Adafruit_PWMServoDriver.h>

// Banner
#define BANNER "ServoShield v1.0"

// PIN definitions
#define DCC_PIN         2
#define ACK_PIN         3
#define BTN_PROGRAM_PIN A0
#define BTN_DOWN_PIN    A1
#define BTN_UP_PIN      A2
#define PROGRAM_LED_PIN A3

// OLED display
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   32
#define SCREEN_ADDRESS  0x3C
#define OLED_RESET      -1

// PWMServo library
#define SERVO_FREQ      50
#define SERVO_MIN_US    600
#define SERVO_MAX_US    2400

// CV
#define CV_BASE_ADDRESS         100
#define OUT_SLOT_SIZE           4
#define POSITION_A_OFFSET       0
#define POSITION_B_OFFSET       1
#define SPEED_OFFSET            2
#define LAST_POSITION_OFFSET    3
#define DEFAULT_DECODER_ADDRESS 4
#define DEFAULT_POSITION_A      50
#define DEFAULT_POSITION_B      200
#define DEFAULT_SPEED           20
#define DEFAULT_LAST_POSITION   0


// Global structs
struct Button {

  int lastState;
  int currentState;
  unsigned long pressedTime;
  unsigned long releasedTime;
};

struct ServoOutput {

  byte positionA;
  byte positionB;
  byte speed;

  byte currentPosition;
  byte targetPosition;
  int stepDelay;
  long lastStepTime;
  bool moving;
  bool directionForward; 
};

// Global objects
NmraDcc Dcc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_PWMServoDriver pwmServo = Adafruit_PWMServoDriver();
Button btnProgram = {HIGH, HIGH, 0, 0};
Button btnUp = {HIGH, HIGH, 0, 0};
Button btnDown = {HIGH, HIGH, 0, 0};
ServoOutput servoOutputs[16];

// Global variables
char buffer[10];
bool inProgramMode = true;
bool programmingAddress = false;
bool sweepOutput = false;
int newAddressValue = 0;
int sweepSteps = 0;
int outIndex = 0;
int outSubIndex = 0;
int usIncrementPerStep = (SERVO_MAX_US - SERVO_MIN_US) / 255;