#include <SPI.h>
#include <RF24.h>

#define SERIAL_DEBUG true

RF24 radio(9,10);
const uint64_t writingPipe = 0xF0F0F0F0E1LL;
const byte address[6] = "00001";
uint16_t values[8];
uint16_t switch = 2;
struct packet {
  uint8_t throttle;
  uint8_t yaw;
  uint8_t pitch;
  uint8_t roll;
} packet;

uint16_t pitch_mid = 537; // higher means more backward
uint16_t roll_mid = 492; // higher means more right

void setup() {
   // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  pinMode(2, INPUT);

  
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  values[0] = map(analogRead(0), 0, 1023, 0, 255);
  values[1] = map(analogRead(1), 0, 1023, 0, 255);
  values[2] = map(1023, 0, 1023, 0, 255);
  values[3] = map(analogRead(2), 0, 1023,0, 255);
  values[4] = 0;
  values[5] = 0;
  values[6] = 0;
  values[7] = 0;
//  packet.throttle = map(analogRead(2), 0, 1023,0, 255);
//  packet.yaw      = map(0, 0, 1023, 255, 0);
//  packet.pitch = map(analogRead(1), 0, 1023, 0, 255);
//  packet.roll = map(analogRead(0), 0, 1023, 0, 255);

  radio.write(&values, sizeof(values));

//  if (SERIAL_DEBUG) {
//    Serial.print(packet.throttle);
//    Serial.print(" ");
//    Serial.print(packet.yaw);
//    Serial.print(" ");
//    Serial.print(packet.pitch);
//    Serial.print(" ");
//    Serial.print(packet.roll);
//    Serial.println();
//  }
}
