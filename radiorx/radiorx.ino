// rf69 mask reciever
// -*- mode: C++ -*-
// Critical Making s17

#include <SPI.h>
#include <Servo.h>
#include <RH_RF69.h>

/************ Motor Setup ***************/

#define servo            13

const int buttonPin = 7;
const int servoPin = 13;
const int maxServoPosition = 180; //degrees - maximum angle from 0 degrees the servo will reach
const int servoDelay = 50; //ms - delay between each 1 degree incriment

int buttonState = 0;         // variable for reading the pushbutton status
int servoState;

//Servo Variable Declaration
Servo myservo;  // create servo object to control a servo
int pos;    // variable to store the servo position

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

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup()
{
  Serial.begin(115200);

  // wait until serial console is open, remove if not tethered to computer
  while (!Serial) delay(1);

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

  // SERVO MOTOR SETUP

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  //Servo Setup Code
  myservo.attach(servoPin);  // attaches the servo on pin to the servo object
}


void loop() {
  int msg = readMessage();
  Serial.println(msg);

  if (msg == 0) {
    controlAudio(0); // turn off audio
    controlServo(0); // move motors down
  } else if (msg == 1) {
    controlAudio(1); // turn on audio
    controlServo(1); // move motors up
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
      if (!len) return -1;
      buf[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

      if (buf[0] == '0') {
        return 0;
      } else if (buf[0] == '1') {
        return 1;
      } else {
        Serial.print("unexpected msg recieved:");
        Serial.println((char*)buf);
        return -1;
      }

    }
  }
}

// control the servo motor, turning it either off or on.
// this code is modified from jimmy's original function.
// pass in the level to the servo to tell it what to do

void controlServo(int lvl) {

  Serial.print("servo target: ");
  Serial.println(lvl);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (lvl) {

    if (servoState == 0) { // already engaged

      //Servo Loop Code, goes from 0 degrees to 180 degrees
      for (pos = 0; pos <= maxServoPosition; pos += 1) {
        Serial.print("pos variable: ");
        Serial.println(pos);             // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(servoDelay);               // waits 15ms for the servo to reach the position
      }

      servoState = 1;
      delay (1000);
    }

  } else { // lvl == 0

    if (servoState == 1) { // already disengaged

      // goes from 180 degrees to 0 degrees
      for (pos = maxServoPosition; pos >= 0; pos -= 1) {
        Serial.print("pos variable: ");
        Serial.println(pos);
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(servoDelay);              // waits 15ms for the servo to reach the position
      }

      servoState = 0;
      delay (1000);
    }
  }

}

// function to set the audio to be on or off given the input level.

void controlAudio(int lvl) {
  // maybe do some other stuff too.
  digitalWrite(audio, lvl);
}

