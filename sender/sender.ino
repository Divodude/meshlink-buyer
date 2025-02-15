#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Pin definitions
#define CE_PIN D4
#define CSN_PIN D2

// Radio configuration
RF24 radio(CE_PIN, CSN_PIN);
const byte addresses[][6] = {"Node2", "Node1"};  // Two addresses for bidirectional communication

// Constants
const uint8_t PAYLOAD_SIZE = 32;

void setup() {
    // Initialize serial communication
    Serial.begin(460800);
    
    // Initialize radio
    if (!radio.begin()) {
        Serial.println("Radio hardware not responding!");
        while (1) {}  // Don't proceed if radio hardware is not working
    }
    
    // Configure radio
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_2MBPS);
    radio.setPayloadSize(PAYLOAD_SIZE);
    
    // Open pipes for bidirectional communication
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
    
    // Start in receiving mode
    radio.startListening();
}

void loop() {
    // Check for incoming serial data to transmit
    if (Serial.available()) {
        // Switch to transmit mode
        radio.stopListening();
        
        // Read from serial and send via radio
        char txBuffer[PAYLOAD_SIZE] = {0};
        int bytesRead = Serial.readBytes(txBuffer, PAYLOAD_SIZE);
        
        bool success = radio.write(txBuffer, PAYLOAD_SIZE);
        
        // Switch back to receive mode
        radio.startListening();
        
        delay(1); // Small delay to allow radio to switch modes
    }
    
    // Check for incoming radio data
    if (radio.available()) {
        char rxBuffer[PAYLOAD_SIZE] = {0};
        
        // Read the payload
        radio.read(rxBuffer, PAYLOAD_SIZE);
        
        // Send to serial port
        Serial.write(rxBuffer, PAYLOAD_SIZE);
    }
}
