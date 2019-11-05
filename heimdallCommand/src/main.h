#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

struct RadioData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte dial1;
  byte dial2;
  byte switches; // bitflag
};

struct AckPayload {
  float lat;
  float lon;
  int16_t heading;
  int16_t pitch;
  int16_t roll;
  int32_t alt;
  byte flags;
};

extern RadioData radioData;
extern AckPayload ackPayload;

void readSwitches();
void resetRadioData();
void writeDataToSerial();
void cleanData();
void readSticks();
void detectPress(int pin, int input);
void printSticks();
