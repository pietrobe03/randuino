#include <MD5.h>
/*
 * TO USE THIS, THE MD5-LIBRARY FOR ARDUINO IS NEEDED:
 * https://github.com/tzikis/ArduinoMD5
 */
#include <QueueList.h>
#include <string.h>

// Definition of output pins
int led1Pin = 13;
int led2Pin = 12;
int led3Pin = 11;

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
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(inpPin, INPUT);
}

// Periodically running code
void loop() {

  static String salt = "00";
  
  // Parse serial commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    // Command parser ("command" => answer)
    if (input.equals("single")) { // "single" => Random number

        // Put together the analog value and the salt
        char* temp;
        sprintf(temp, "%i%s", analogRead(inpPin), salt.c_str());

        // Hash it
        unsigned char* hash = MD5::make_hash(temp);
        char* md5chr = MD5::make_digest(hash, 16);
        
        // Extract new salt and random number
        String rng = String(md5chr[0]) + String(md5chr[1]);
        salt = String(md5chr[random(0,15)]) + String(md5chr[random(0,15)]); // Pseudo-random numbers create additional randomness for the salt

        // Convert number from hex to dec and send it to the client 
        Serial.print(strtol(rng.c_str(), NULL, 16) + "\n");

        // Free some memory
        free(temp);
        free(md5chr);
        free(hash);
        
        // Flash green led
        digitalWrite(led3Pin, HIGH);
        delay(50);
        digitalWrite(led3Pin, LOW);
      
    } else if (input.equals("connect")) { // "connect" => "randuino"
        Serial.print("randuino\n");

        // Flash red led
        digitalWrite(led1Pin, HIGH);
        delay(50);
        digitalWrite(led1Pin, LOW);
        
    } else if (input.equals("heartbeat")) { // "heartbeat" => "still_alive"
        Serial.print("still_alive\n");

    } else {
      Serial.print("CMD_ERR: " + input + "\n");
      
    }
  } 

}
