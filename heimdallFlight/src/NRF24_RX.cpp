
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
static const uint64_t pipe = 0xE8E8F0F0E1LL;

RF24 radio(7, 8); // CE, CSN

RF24Data nrf24Data;
RF24AckPayload nrf24AckPayload;
extern RF24AckPayload nrf24AckPayload;

void resetRF24Data()
{
  nrf24Data.throttle = 0;
  nrf24Data.yaw = 128;
  nrf24Data.pitch = 128;
  nrf24Data.roll = 128;
  nrf24Data.dial1 = 0;
  nrf24Data.dial2 = 0;
  nrf24Data.switches = 0;
}

void resetRF24AckPayload()
{
  nrf24AckPayload.lat = 0;
  nrf24AckPayload.lon = 0;
  nrf24AckPayload.heading = 0;
  nrf24AckPayload.pitch = 0;
  nrf24AckPayload.roll = 0;
  nrf24AckPayload.alt = 0;
  nrf24AckPayload.flags = 0;
}

void NRF24_Init() {

  resetRF24Data();
  resetRF24AckPayload();

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();

  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

void NRF24_Read_RC() {

  static unsigned long lastRecvTime = 0;

  nrf24AckPayload.lat = 35.62;
  nrf24AckPayload.lon = 139.68;
  nrf24AckPayload.heading = att.heading;
  nrf24AckPayload.pitch = att.angle[PITCH];
  nrf24AckPayload.roll = att.angle[ROLL];
  nrf24AckPayload.alt = alt.EstAlt;
  memcpy(&nrf24AckPayload.flags, &f, 1); // first byte of status flags

  unsigned long now = millis();
  while ( radio.available() ) {
    radio.writeAckPayload(1, &nrf24AckPayload, sizeof(RF24AckPayload));
    radio.read(&nrf24Data, sizeof(RF24Data));
    lastRecvTime = now;
  }
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    resetRF24Data();
  }

  nrf24_rcData[THROTTLE] = map(nrf24Data.throttle, 0, 255, 1000, 2000);
  nrf24_rcData[YAW] =      map(nrf24Data.yaw,      0, 255, 1000, 2000);
  nrf24_rcData[PITCH] =    map(nrf24Data.pitch,    0, 255, 1000, 2000);
  nrf24_rcData[ROLL] =     map(nrf24Data.roll,     0, 255, 1000, 2000);

}

#endif
