#include <QueueList.h>
#include <string.h>

// Definition of output pins
int beeper = A1;
int greenLED = 7;
int redLED = 8;
int blueLED = 9;

// Definition of input pin
int inpPin = A0;

// QueueList (FIFO) buffers generated numbers
QueueList<short> numbers;


// Setup code runs once at initialization
void setup() {
  
  // Setup of serial communication
  Serial.begin(9600);
  Serial.setTimeout(100);

  // Initialization of pins
  pinMode(beeper, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(inpPin, INPUT);
}

// Periodically running code
void loop() {
  
  // Parse serial commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    // Command parser ("command" => answer)
    if (input.equals("single")) { // "single" => Random number
        
        analogWrite(beeper, 128); // Start beeping

        int num = 0;
        
        // Measure the voltage of the input pin (random) and convert it into a random number
        for (int i = 0; i < 8; i++) {
          int t = 0;
          for (int j = 0; j < 8; j++) {
            delay(2);
            t += analogRead(inpPin);
          }
          num = num * 2 + (t % 2);
        }
        Serial.print((String) num + "\n");
        analogWrite(beeper, 0); // Stop beeping
        digitalWrite(greenLED, HIGH); // Flash green LED
        delay(15);
        digitalWrite(greenLED, LOW);
    } else if (input.equals("connect")) { // "connect" => "randuino"
        Serial.print("randuino\n");

        // Flash blue led
        digitalWrite(blueLED, HIGH);
        delay(50);
        digitalWrite(blueLED, LOW);
        
    } else if (input.equals("heartbeat")) { // "heartbeat" => "still_alive"
        Serial.print("still_alive\n");

    } else { // Unknown command
      Serial.print("CMD_ERR: " + input + "\n");
      // Flash red led
      digitalWrite(redLED, HIGH);
      delay(50);
      digitalWrite(redLED, LOW);
      
    }
  }
}
