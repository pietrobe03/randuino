#include <QueueList.h>

// Definition of output pins
int led1Pin = 13;
int led2Pin = 12;
int led3Pin = 11;

// Definition of input pin
int inpPin = 9;

// Shuffled array containing all digits
int ranList[] = {6,8,1,5,4,7,0,2,9,3};

// QueueList (FIFO) buffers generated numbers
QueueList<short> numbers;


// Setup code runs once at initialization
void setup() {
  
  // Setup of serial communication
  Serial.begin(9600);
  Serial.setTimeout(100);

  // Initialization of pins
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(inpPin, INPUT);
  pinMode(A0, INPUT);
}

// Periodically ran code
void loop() {
  
  // Stores current iteration index as a digit between 0 and 9
  static unsigned short number = 0;
  
  // Stores if input signal existed in previous iteration
  static bool active = false;

  // Checking for a new input signal
  bool pressed = (digitalRead(inpPin) == HIGH);
  if (!active && pressed && numbers.count() < 512) {
    digitalWrite(led2Pin, HIGH);
    numbers.push(number);
    delay(50);
    digitalWrite(led2Pin, LOW);
  }
  active = pressed;
  
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
        digitalWrite(led3Pin, HIGH);
        delay(50);
        digitalWrite(led3Pin, LOW);
      }
      
    } else if (input.equals("connect")) { // "connect" => "randuino"
        Serial.print("randuino\n");

        // Flash red led
        digitalWrite(led1Pin, HIGH);
        delay(50);
        digitalWrite(led1Pin, LOW);
        
    } else if (input.equals("buffer")) { // "buffer" => number of buffered elements
        Serial.print("B:" + (String) numbers.count() + "\n");
        
    } else if (input.equals("heartbeat")) { // "heartbeat" => "still_alive"
        Serial.print("still_alive\n");

    } else {
      Serial.print("CMD_ERR: " + input + "\n");
      
    }
  } 

  // Iterate to next number between 0 and 9
  number++;
  if (number == 256) {
    number = 0;
  } 
}
