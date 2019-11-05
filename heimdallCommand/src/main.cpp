#include "main.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define THROTTLE A0
#define YAW 	 A1
#define PITCH    A2
#define ROLL	 A3

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(9, 10);

const byte serialHeader[4] = {255,254,253,252};

struct RadioData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte dial1;
  byte dial2;
  byte switches; // bitflag
};

RadioData radioData;

struct AckPayload {
  float lat;
  float lon;
  int16_t heading;
  int16_t pitch;
  int16_t roll;
  int32_t alt;
  byte flags;
};

AckPayload ackPayload;


int pins[] = {12, 11, 10, 9, 8, 7, 6, 5};
int buttons[] = {false, false, false, false,  false,  false,  false,  false};
int sticks[] = {0, 0, 0, 0};
String names[] = {"THROTTLE UP", "THROTTLE DOWN","YAW UP", "YAW DOWN", "PITCH UP", "PITCH DOWN", "ROLL UP", "ROLL DOWN"};

int input;

void setup()
{
  Serial.begin(9600);
  printf_begin();
  pinMode(13, OUTPUT);

  for(int i = 0; i < sizeof(pins)/sizeof(pins[0]); i++){
    pinMode(pins[i], INPUT_PULLUP);
  }

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
  for(int i = 0; i< sizeof(pins)/sizeof(pins[0]); i++){
    input = digitalRead(pins[i]);
    detectPress(i, input);
  }

  readSticks();
  //printSticks();

    radioData.switches = 0;
  if ( ! digitalRead(4) ) radioData.switches |= 0x1;
  if ( ! digitalRead(2) ) radioData.switches |= 0x2;

  radio.write(&radioData, sizeof(RadioData));

  while ( radio.isAckPayloadAvailable()) {
    radio.read(&ackPayload, sizeof(AckPayload));
  }

  //adjustAckPayloadValues();
  writeDataToSerial();
}

void resetRadioData()
{
  radioData.throttle = 1270;
  radioData.yaw = 127;
  radioData.pitch = 127;
  radioData.roll = 127;
  radioData.dial1 = 0;
  radioData.dial2 = 0;
  radioData.switches = 0;
}

void writeDataToSerial()
{
  cleanData();

  Serial.print("  Pitch "); Serial.print(ackPayload.pitch);
  Serial.print("  Roll ");  Serial.print(ackPayload.roll);
  Serial.print("  Yaw ");   Serial.print(ackPayload.heading);
  Serial.print("  Alt ");   Serial.print(ackPayload.alt);
  Serial.print("  Flags "); Serial.print(ackPayload.flags);
  Serial.println();

  /*
  Serial.write(serialHeader, 4);
  Serial.write((uint8_t*)&radioData, sizeof(RadioData));
  Serial.write((uint8_t*)&ackPayload, sizeof(AckPayload));
  */
}

/**************************************************/

void cleanData()
{
  if ( radioData.throttle == 255 ) radioData.throttle = 254;
  if ( radioData.yaw == 255 ) radioData.yaw = 254;
  if ( radioData.pitch == 255 ) radioData.pitch = 254;
  if ( radioData.roll == 255 ) radioData.roll = 254;
  if ( radioData.switches == 255 ) radioData.switches = 254;

  if ( ackPayload.alt == 255 ) ackPayload.alt = 254; // not quite sufficient, but unlikely that any other bytes of the altitude will be 255
  ackPayload.flags &= 0x2; // ok because we are only interested in bit 2 (ARMED)
}

void readSticks(){
  sticks[0] = analogRead(THROTTLE);
  sticks[1] = analogRead(YAW);
  sticks[2] = analogRead(PITCH);
  sticks[3] = analogRead(ROLL);
  /*
  radioData.throttle = mapJoystickValues( analogRead(A0), 180, 497, 807, false );
  radioData.yaw      = mapJoystickValues( analogRead(A1), 109, 460, 848, false );
  radioData.pitch    = mapJoystickValues( analogRead(A2), 136, 462, 779, false );
  radioData.roll     = mapJoystickValues( analogRead(A3), 165, 564, 906, true );
  */
}

void detectPress(int pin, int input){
  if(input == 1 && buttons[pin]){
    Serial.print(names[pin]);
    Serial.println(" was pressed and released");
    buttons[pin] = false;
  }

  if(input == 0 && !buttons[pin]){
  	buttons[pin] = true;
  }
}

void printSticks(){
  Serial.print("THROTTLE: ");
  Serial.print(sticks[0]);
  Serial.print(", YAW: ");
  Serial.print(sticks[1]);
  Serial.print(", PITCH: ");
  Serial.print(sticks[2]);
  Serial.print(", ROLL: ");
  Serial.println(sticks[3]);
}
