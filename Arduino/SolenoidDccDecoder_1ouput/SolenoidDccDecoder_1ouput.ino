// DCC Decoder for solenoid motors, 2 outputs
// Luca Dentella
//
// Playlist of my tutorials: https://youtube.com/playlist?list=PL45uNmSXju7mn3bMtVVPlFQmpD3mUI5mi


#include <NmraDcc.h>

#define DCC_PIN 2
#define ACK_PIN 3

#define DECODER_ADDR 4

#define IMPULSE_DURATION 200
#define PIN_STRAIGHT  4
#define PIN_DIVERGING 5

NmraDcc Dcc;

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {

  if(Addr == DECODER_ADDR) {
    if(Direction == 0) {
      digitalWrite(PIN_STRAIGHT, HIGH);
      delay(IMPULSE_DURATION);
      digitalWrite(PIN_STRAIGHT, LOW);
    } else {
      digitalWrite(PIN_DIVERGING, HIGH);
      delay(IMPULSE_DURATION);
      digitalWrite(PIN_DIVERGING, LOW);    
    }
  }

  Serial.print("Output moved to position ");
  Serial.println(Direction);
}

void notifyCVAck(void) {

  digitalWrite(ACK_PIN, HIGH);
  delay(8);  
  digitalWrite(ACK_PIN, LOW);
}

void setup() {

  Serial.begin(115200);
  pinMode(ACK_PIN, OUTPUT);
  pinMode(PIN_STRAIGHT, OUTPUT);
  pinMode(PIN_DIVERGING, OUTPUT); 
  Dcc.pin(digitalPinToInterrupt(DCC_PIN), DCC_PIN, false);
  Dcc.init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DECODER_ADDR & 0xFF);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DECODER_ADDR >> 8);
}

void loop() {

  Dcc.process();
}
