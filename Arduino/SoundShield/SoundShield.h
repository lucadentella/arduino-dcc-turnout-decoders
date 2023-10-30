// Libraries
#include <NmraDcc.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>

// Banner
#define BANNER "SoundShield v1.0"

// PIN definitions
#define DCC_PIN         2
#define ACK_PIN         3
#define MP3TX_PIN       8
#define MP3RX_PIN       9
#define BTN_PROGRAM_PIN A0
#define PROGRAM_LED_PIN A3

// Play mode
#define PLAY_SINGLE     0
#define PLAY_LOOP       1

// CV
#define CV_BASE_ADDRESS         101
#define DEFAULT_DECODER_ADDRESS 4
#define DEFAULT_PLAY_MODE       PLAY_SINGLE
#define DEFAULT_VOLUME          15

// For DecoderPro productId 257 (https://www.jmri.org/help/en/html/apps/DecoderPro/CreateDecoderAdvanced.shtml)
#define CV47_COUNTRY_CODE       0
#define CV48_DEVELOPER_CODE     0
#define CV49_THIRD_PRODUCT_ID   1 // Arduino Shields
#define CV50_FOURTH_PRODUCT_ID  1 // SoundShield

// Global objects
NmraDcc Dcc;
SoftwareSerial mySerial(MP3TX_PIN, MP3RX_PIN);
JQ6500_Serial mp3(mySerial);

// Global variables
unsigned int fileCount;
bool btnProgramDown = false;
bool inProgrammingMode = false;