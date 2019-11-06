struct RadioData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte dial1;
  byte dial2;
  byte trim; // bitflag
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
void readButtons();
void readTrim();
void readSwitches();
void resetRadioData();
void cleanData();
void readSticks();
void detectPresses();
void detectPress(int pin, int input);
void printSticks();
void writeDataToSerial();
