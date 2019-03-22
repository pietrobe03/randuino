#include <QueueList.h>

// Definition of output pins
int greenLED = 7;
int redLED = 8;
int blueLED = 9;

// Definityion of input pin
int inpPin = 10;

// QueueList (FIFO) buffers generated numbers
QueueList<unsigned short> numbers;

// Setup code runs once at initialization
void setup() {

  // Setup of serial communication
  Serial.begin(9600);
  Serial.setTimeout(100);

  // Initialization of pins
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(inpPin, INPUT);
}

// Periodically running code
void loop() {
  
  // Stores current iteration index as a digit between 0 and 255
  static unsigned short number = 0;
  
  // Stores if input signal existed in previous iteration
  static bool active = false;

  // Checking for a new input signal
  bool pressed = (digitalRead(inpPin) == HIGH);

  if (!active && pressed) {
    if (numbers.count() < 256) {
      numbers.push(number);
    }
  }
  active = pressed;

  digitalWrite(blueLED, (numbers.count() > 255 ? HIGH : LOW));
  
  // Parse serial commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    // Command parser ("command" => answer)
    if (input.equals("single")) { // "single" => Random number or "buffer_empty"
      if (numbers.isEmpty()) {
        Serial.print("buffer_empty\n");
      } else {
        Serial.print((String) numbers.pop() + "\n");
        
        // Flash green led
        digitalWrite(greenLED, HIGH);
        delay(5);
        digitalWrite(greenLED, LOW);
      }
      
    } else if (input.equals("connect")) { // "connect" => "randuino"
        Serial.print("randuino\n");

        // Flash blue led
        digitalWrite(blueLED, HIGH);
        delay(50);
        digitalWrite(blueLED, LOW);
        
    } else if (input.equals("buffer")) { // "buffer" => number of buffered elements
        Serial.print("B:" + (String) numbers.count() + "\n");
        
    } else if (input.equals("heartbeat")) { // "heartbeat" => "still_alive"
        Serial.print("still_alive\n");

    } else {
      Serial.print("CMD_ERR: " + input + "\n");
      
      // Flash red led
      digitalWrite(redLED, HIGH);
      delay(50);
      digitalWrite(redLED, LOW);
    }
  } 

  // Iterate to next number between 0 and 9
  number++;
  if (number == 256) {
    number = 0;
  } 
}
