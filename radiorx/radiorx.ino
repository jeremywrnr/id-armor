// rf69 mask reciever
// -*- mode: C++ -*-
// Critical Making s17

#include <SPI.h>
#include <RH_RF69.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0
#define RFM69_CS      10   // "B"
#define RFM69_RST     11   // "A"
#define RFM69_IRQ     6    // "D"
#define RFM69_INT     6    // "D"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ)

// used for feather pass through
#define audio            12
#define servo            13

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup()
{
  Serial.begin(115200);

  // wait until serial console is open, remove if not tethered to computer
  // while (!Serial) delay(1);

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }

  Serial.println("RFM69 radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");
  Serial.print((int)RF69_FREQ);
  Serial.println(" MHz");

  // pinmode setting
  pinMode(audio, OUTPUT);
  pinMode(servo, OUTPUT);
}


void loop() {
  int msg = readMessage();

  if (msg >= 0) {
    digitalWrite(audio, msg);
    digitalWrite(servo, msg);
    Serial.println(msg);
  }

}

// function to read whether a signal was received from the handtransmitter
// returns which type of signal was recieved (either high or low), or
int readMessage() {
  if (rf69.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len)
        return -1;
      buf[len] = 0;

      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.print((char*)buf);
      Serial.print("\tRSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

      if (buf[0] == '0') {
        return 0;
      } else if (buf[0] == '1') {
        return 1;
      } else {
        Serial.print("unexpected msg recieved:");
        Serial.println((char*)buf);
      }

    }
  }

  delay(100); // delay before reading again
  return -1;

}

