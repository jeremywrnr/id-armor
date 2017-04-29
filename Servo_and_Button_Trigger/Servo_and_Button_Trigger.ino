/*
Jimmy Huang
04/28/2017

Implement Noise Filter?
*/

const int buttonPin = 7;
const int servoPin = 11;
const int maxServoPosition = 180; //degrees - maximum angle from 0 degrees the servo will reach
const int servoDelay = 50; //ms - delay between each 1 degree incriment

int buttonState = 0;         // variable for reading the pushbutton status
int servoState;

//Servo Variable Declaration
#include <Servo.h>
Servo myservo;  // create servo object to control a servo
int pos;    // variable to store the servo position

void setup() {
  Serial.begin(9600);

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  //Servo Setup Code
  myservo.attach(servoPin);  // attaches the servo on pin to the servo object
}

void loop() {

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    Serial.print("buttonState variable: "); Serial.println(buttonState);

    if (servoState == 1) {}

    else {
      //Servo Loop Code
      for (pos = 0; pos <= maxServoPosition; pos += 1) { // goes from 0 degrees to 180 degrees
        Serial.print("pos variable: "); Serial.println(pos);
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(servoDelay);                       // waits 15ms for the servo to reach the position
      }
      servoState = 1;
      delay (1000);
    }
  }

  else {
    Serial.print("buttonState variable: "); Serial.println(buttonState);

    if (servoState == 0) {}

    else {
      for (pos = maxServoPosition; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        Serial.print("pos variable: "); Serial.println(pos);
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(servoDelay);                       // waits 15ms for the servo to reach the position
      }
      servoState = 0;
      delay (1000);
    }
  }
}



