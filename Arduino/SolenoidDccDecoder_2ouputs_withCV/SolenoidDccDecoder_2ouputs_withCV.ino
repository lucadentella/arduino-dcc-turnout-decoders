// DCC Decoder for solenoid motors, 2 outputs and CV30 to set impulse duration
// Luca Dentella
//
// Playlist of my tutorials: https://youtube.com/playlist?list=PL45uNmSXju7mn3bMtVVPlFQmpD3mUI5mi


#include <NmraDcc.h>

#define DCC_PIN 2
#define ACK_PIN 3

#define DECODER_ADDR 4

#define CV_IMPULSE_DURATION      30
#define DEFAULT_IMPULSE_DURATION 200

#define OUT1_PIN_STRAIGHT  4
#define OUT1_PIN_DIVERGING 5
#define OUT2_PIN_STRAIGHT  6
#define OUT2_PIN_DIVERGING 7

NmraDcc Dcc;

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {

  // Get the current decoder address
  uint16_t myAddress = Dcc.getAddr();

  // Get impulse duration from CV
  uint8_t impulseDuration = Dcc.getCV(CV_IMPULSE_DURATION);

  // Output 1
  if(Addr == myAddress) {
    if(Direction == 0) {
      digitalWrite(OUT1_PIN_STRAIGHT, HIGH);
      delay(impulseDuration);
      digitalWrite(OUT1_PIN_STRAIGHT, LOW);
    } else {
      digitalWrite(OUT1_PIN_DIVERGING, HIGH);
      delay(impulseDuration);
      digitalWrite(OUT1_PIN_DIVERGING, LOW);    
    }
    Serial.print("Output 1 moved to position ");
    Serial.println(Direction);    
  }

  // Output 2
  else if(Addr == myAddress + 1) {
    if(Direction == 0) {
      digitalWrite(OUT2_PIN_STRAIGHT, HIGH);
      delay(impulseDuration);
      digitalWrite(OUT2_PIN_STRAIGHT, LOW);
    } else {
      digitalWrite(OUT2_PIN_DIVERGING, HIGH);
      delay(impulseDuration);
      digitalWrite(OUT2_PIN_DIVERGING, LOW);    
    }
    Serial.print("Output 2 moved to position ");
    Serial.println(Direction);     
  }
}

void notifyCVAck(void) {

  digitalWrite(ACK_PIN, HIGH);
  delay(8);  
  digitalWrite(ACK_PIN, LOW);
}

void notifyCVResetFactoryDefault() {

  // Reset requested, set default address...
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DECODER_ADDR & 0xFF);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DECODER_ADDR >> 8);  

  // ...and impulse duration
  Dcc.setCV(CV_IMPULSE_DURATION, DEFAULT_IMPULSE_DURATION);
}

void setup() {

  Serial.begin(115200);
  pinMode(ACK_PIN, OUTPUT);
  pinMode(OUT1_PIN_STRAIGHT, OUTPUT);
  pinMode(OUT1_PIN_DIVERGING, OUTPUT);
  pinMode(OUT2_PIN_STRAIGHT, OUTPUT);
  pinMode(OUT2_PIN_DIVERGING, OUTPUT);   
  Dcc.pin(digitalPinToInterrupt(DCC_PIN), DCC_PIN, false);
  Dcc.init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);
}

void loop() {

  Dcc.process();
}
