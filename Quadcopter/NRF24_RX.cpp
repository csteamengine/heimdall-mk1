
#include "Arduino.h"
#include "config.h"
#include "def.h"
#include "types.h"
#include "MultiWii.h"
#include <RF24.h>
#include "NRF24_RX.h"

#if defined(NRF24_RX)

int16_t nrf24_rcData[RC_CHANS];

// Single radio pipe address for the 2 nodes to communicate.
static const uint64_t pipe = 0xF0F0F0F0E1LL;
const byte address[6] = "00001";
RF24 radio(7, 8); // CE, CSN
uint16_t values[8];

struct packet {
  uint8_t throttle;
  uint8_t yaw;
  uint8_t pitch;
  uint8_t roll;
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

packet nrf24Data;

void resetRF24Data() 
{
  nrf24Data.throttle = 0;
  nrf24Data.yaw = 128;
  nrf24Data.pitch = 128;
  nrf24Data.roll = 128;
}

void NRF24_Init() {

  resetRF24Data();
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void NRF24_Read_RC() {
  static unsigned long lastRecvTime = 0;

  unsigned long now = millis();
  if ( radio.available() ) {
//    TODO replace this with the packet struct
    radio.read(&values, sizeof(values));

    lastRecvTime = now;
//    TODO replace this with the packet struct
    nrf24_rcData[THROTTLE] = map(values[3], 0, 255, 1000, 2000);
    nrf24_rcData[YAW] =      map(values[2],      0, 1024, 1000, 2000);
    nrf24_rcData[PITCH] =    map(values[1],    0, 255, 1000, 2000);
    nrf24_rcData[ROLL] =     map(values[0],     0, 255, 1000, 2000);

  }
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    resetRF24Data();
  }
  
}

#endif

