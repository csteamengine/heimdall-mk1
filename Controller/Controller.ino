#include <SPI.h>
#include <RF24.h>

#define SERIAL_DEBUG true

#define RY A3
#define RX A2
#define LY A1
#define LX A0

#define DUP 2
#define DLEFT 0
#define DDOWN 11
#define DRIGHT 8

#define BUTTONPLUS A4
#define BUTTONMINUS A5

#define BUTTONHOME 7

#define XBUTTON 6
#define CIRCLE 5
#define TRIANGLE 4
#define SQUARE 3

RF24 radio(9,10);
const uint64_t writingPipe = 0xF0F0F0F0E1LL;
const byte address[6] = "00001";

//TODO get radio to work with structs
struct packet {
  uint8_t throttle = 125;
  uint8_t yaw = 125;
  uint8_t pitch = 125;
  uint8_t roll = 125;
  boolean plus = false;
  boolean minus = false;
  boolean homebutton = false;
  boolean dup = false;
  boolean dleft = false;
  boolean ddown = false;
  boolean dright = false;
  boolean xbutton = false;
  boolean circle = false;
  boolean triangle = false;
  boolean square = false;
};
packet data;
uint16_t pitch_mid = 537; // higher means more backward
uint16_t roll_mid = 492; // higher means more right

void setup() {
   // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  pinMode(LX, INPUT);
  pinMode(LY, INPUT);
  pinMode(RX, INPUT);
  pinMode(RY, INPUT);

  pinMode(DUP, INPUT_PULLUP);
  pinMode(DLEFT, INPUT_PULLUP);
  pinMode(DDOWN, INPUT_PULLUP);
  pinMode(DRIGHT, INPUT_PULLUP);

  pinMode(BUTTONPLUS, INPUT_PULLUP);
  pinMode(BUTTONMINUS, INPUT_PULLUP);
  pinMode(BUTTONHOME, INPUT_PULLUP);

  pinMode(XBUTTON, INPUT_PULLUP);
  pinMode(CIRCLE, INPUT_PULLUP);
  pinMode(TRIANGLE, INPUT_PULLUP);
  pinMode(SQUARE, INPUT_PULLUP);
  
  initRadio();
}

void loop() {
  readInputData();
 
//  radio.write(&data, sizeof(data));

  debugInput(data);

//  delay(500);
}

void sendToPi(){
//  TODO send a message to the pi with whatever info I want. 
}

void initRadio(){
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}


void readInputData(){
  data.throttle = map(analogRead(RY), 0, 1023, 0, 255);
  data.yaw = map(analogRead(RX), 0, 1023, 0, 255);
  data.roll = map(analogRead(LX), 0, 1023, 0, 255);
  data.pitch = map(analogRead(LY), 0, 1023, 0, 255); 
  data.dup = readButton(DUP);
  data.dleft = readButton(DLEFT);
  data.ddown = readButton(DDOWN);
  data.dright = readButton(DRIGHT);
  data.xbutton = readButton(XBUTTON);
  data.circle = readButton(CIRCLE);
  data.triangle = readButton(TRIANGLE);
  data.square = readButton(SQUARE);
  data.plus = readButton(BUTTONPLUS);
  data.minus = readButton(BUTTONMINUS);
  data.homebutton = readButton(BUTTONHOME);
}

void debugInput(packet inputs){
  Serial.print("T: ");
  Serial.print(inputs.throttle);
  Serial.print(" - R:");
  Serial.print(inputs.roll);
  Serial.print(" - P:");
  Serial.print(inputs.pitch);
  Serial.print(" - Y:");
  Serial.print(inputs.yaw);
  Serial.print(" - DU:");
  Serial.print(inputs.dup);
  Serial.print(" - DL");
  Serial.print(inputs.dleft);
  Serial.print(" - DD:");
  Serial.print(inputs.ddown);
  Serial.print(" - DR:");
  Serial.print(inputs.dright);
  Serial.print(" - X:");
  Serial.print(inputs.xbutton);
  Serial.print(" - C:");
  Serial.print(inputs.circle);
  Serial.print(" - T:");
  Serial.print(inputs.triangle);
  Serial.print(" - S:");
  Serial.print(inputs.square);
  Serial.print(" - H:");
  Serial.print(inputs.homebutton);
  Serial.print(" - P:");
  Serial.print(inputs.plus);
  Serial.print(" - M:");
  Serial.println(inputs.minus);
}

boolean readButton(uint8_t button){
  uint8_t val = digitalRead(button);
  if(val == 1){
    return false;
  }
  return true;
}

