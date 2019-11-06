#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"
#include "main.h"

//############### OUPUT SETTINGS ################
// #define DEBUG
#define SERIAL_OUTPUT

#define THROTTLE A0
#define YAW 	 A1
#define PITCH    A2
#define ROLL	 A3
#define THROTTLE_TRIM_UP 12
#define THROTTLE_TRIM_DOWN 11
#define YAW_TRIM_UP 10
#define YAW_TRIM_DOWN 9
#define PITCH_TRIM_UP 8
#define PITCH_TRIM_DOWN 7
#define ROLL_TRIM_UP 6
#define ROLL_TRIM_DOWN 5
#define ARMED_PIN 13

const uint64_t pipeOut = 0xE8E8F0F0E1LL;
const byte setBits[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};


RF24 radio(9, 10);

const byte serialHeader[4] = {255,254,253,252};

RadioData radioData;
AckPayload ackPayload;

int pins[] = {
  THROTTLE_TRIM_UP,
  THROTTLE_TRIM_DOWN,
  YAW_TRIM_UP,
  YAW_TRIM_DOWN,
  PITCH_TRIM_UP,
  PITCH_TRIM_DOWN,
  ROLL_TRIM_UP,
  ROLL_TRIM_DOWN
};

byte trimButtons = 0;
byte trimTemp = 0;
int sticks[] = {0, 0, 0, 0};

int input;

void setup()
{
  //Init Serial Stuff
  Serial.begin(9600);
  printf_begin();

  //Init Ouput Pins
  pinMode(ARMED_PIN, OUTPUT);

  //Init Input Pins
  for(uint16_t i = 0; i < sizeof(pins)/sizeof(pins[0]); i++){
    pinMode(pins[i], INPUT_PULLUP);
  }

  //Init Radio Stuff
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.openWritingPipe(pipeOut);
  resetRadioData();
  radio.printDetails();

}

void loop()
{
  detectPresses();

  readSticks();
  //printSticks();

  readTrim();
  readButtons();
  readSwitches();

  radio.write(&radioData, sizeof(RadioData));

  while (radio.isAckPayloadAvailable()) {
    radio.read(&ackPayload, sizeof(AckPayload));
  }

  //TODO potentially set a color for RGB LED
  if(ackPayload.flags & 0x02){
    digitalWrite(ARMED_PIN, HIGH);
  }else{
    digitalWrite(ARMED_PIN, LOW);
  }

  //adjustAckPayloadValues();
  #if defined(SERIAL_OUTPUT)
    writeDataToSerial();
  #endif
}

void readTrim(){
  radioData.trim = 0;
  for(int i = 0; i<8; i++){
    if(trimButtons & setBits[i]) radioData.trim |= setBits[i];
  }
}

void readButtons(){
  // radioData.switches = 0;
  // if ( ! digitalRead(4) ) radioData.switches |= 0x1;
  // if ( ! digitalRead(2) ) radioData.switches |= 0x2;
}

//TODO Update this method to handle button presses and switches
void readSwitches(){
  // radioData.switches = 0;
  // if ( ! digitalRead(4) ) radioData.switches |= 0x1;
  // if ( ! digitalRead(2) ) radioData.switches |= 0x2;
}

void resetRadioData()
{
  //TODO calibrate these values
  radioData.throttle = 255;
  radioData.yaw = 127;
  radioData.pitch = 127;
  radioData.roll = 127;
  radioData.dial1 = 0;
  radioData.dial2 = 0;
  radioData.trim = 0;
}

/**************************************************/

void cleanData()
{
  if ( radioData.throttle == 255 ) radioData.throttle = 254;
  if ( radioData.yaw == 255 ) radioData.yaw = 254;
  if ( radioData.pitch == 255 ) radioData.pitch = 254;
  if ( radioData.roll == 255 ) radioData.roll = 254;
  if ( radioData.trim == 255 ) radioData.trim = 254;

  if ( ackPayload.alt == 255 ) ackPayload.alt = 254; // not quite sufficient, but unlikely that any other bytes of the altitude will be 255

  //Armed is the second bit in flags_struct_t
  ackPayload.flags &= 0x2; // ok because we are only interested in bit 2 (ARMED)
}

void readSticks(){
  //TODO map these values with calibration data
  radioData.throttle = analogRead(THROTTLE);
  radioData.yaw = analogRead(YAW);
  radioData.pitch = analogRead(PITCH);
  radioData.roll = analogRead(ROLL);
  /*
  radioData.throttle = mapJoystickValues( analogRead(A0), 180, 497, 807, false );
  radioData.yaw      = mapJoystickValues( analogRead(A1), 109, 460, 848, false );
  radioData.pitch    = mapJoystickValues( analogRead(A2), 136, 462, 779, false );
  radioData.roll     = mapJoystickValues( analogRead(A3), 165, 564, 906, true );
  */
}

void detectPresses(){
  trimButtons = 0;
  for(uint16_t i = 0; i < sizeof(pins)/sizeof(pins[0]); i++){
    input = digitalRead(pins[i]);
    detectPress(i, input);
  }
}

void detectPress(int pin, int input){
  if(input == 1 && (trimTemp & setBits[pin])){
    trimTemp ^= setBits[pin];
    trimButtons |= setBits[pin];
  }
  if(input == 0 && !(trimTemp & setBits[pin])){
  	trimTemp ^= setBits[pin];
  }
}


void printSticks(){
  #if defined(DEBUG)
    Serial.print("THROTTLE: ");
    Serial.print(radioData.throttle );
    Serial.print(", YAW: ");
    Serial.print(radioData.yaw );
    Serial.print(", PITCH: ");
    Serial.print(radioData.pitch );
    Serial.print(", ROLL: ");
    Serial.println(radioData.roll );
  #endif
}



void writeDataToSerial()
{
  #if defined(SERIAL_OUTPUT)
    cleanData();

    #if defined(DEBUG)
      Serial.print("  Pitch "); Serial.print(ackPayload.pitch);
      Serial.print("  Roll ");  Serial.print(ackPayload.roll);
      Serial.print("  Yaw ");   Serial.print(ackPayload.heading);
      Serial.print("  Alt ");   Serial.print(ackPayload.alt);
      Serial.print("  Flags "); Serial.print(ackPayload.flags);
      Serial.println();
    #else
      Serial.write(serialHeader, 4);
      Serial.write((uint8_t*)&radioData, sizeof(RadioData));
      Serial.write((uint8_t*)&ackPayload, sizeof(AckPayload));
    #endif
  #endif

}
