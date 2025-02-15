#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>

// Define pins for RF24 module
RF24 radio(4, 2);  // CE, CSN pins
RF24Audio rfAudio(radio, 0);

void setup() {
  Serial.begin(115200);
  
  // Initialize the radio
  if (!radio.begin()) {
    Serial.println("Radio initialization failed!");
    while (1);
  }
  
  // Initialize RF24Audio
  rfAudio.begin();
  
  // Set to transmit mode
  rfAudio.transmit();
  
  Serial.println("Transmitter initialized");
}

void loop() {
  // Continuously transmit audio
  // The RF24Audio library handles the audio transmission automatically
}