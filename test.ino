#include <RF24.h>
#include <nRF24L01.h>

// Motor Driver 1 (L298N - Left Side)
#define ENA 3   // Speed control for Left motors
#define IN1 2   // Direction control for Left motors
#define IN2 4

// Motor Driver 2 (L298N - Right Side)
#define ENB 6   // Speed control for Right motors
#define IN3 7   // Direction control for Right motors
#define IN4 8

// Define NRF24L01 pins
RF24 radio(9, 10);
const byte address[6] = "00001";

// Variables for timeout
unsigned long lastMessageTime = 0;
const unsigned long timeoutDuration = 2000; // 2 seconds timeout

void setup() {
    Serial.begin(9600);

    // Initialize NRF24L01
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_LOW);
    radio.setChannel(108); // Ensure transmitter and receiver match
    radio.startListening(); // Set as receiver
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setAutoAck(true);
    radio.setRetries(5, 15);
    Serial.println("Receiver ready");

    // Initialize motor driver pins
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    stopMotors(); // Ensure motors are stopped initially
}

void moveForward(int speed) {
    analogWrite(ENA, speed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    analogWrite(ENB, speed);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void moveBackward(int speed) {
    analogWrite(ENA, speed);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    analogWrite(ENB, speed);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void turnLeft(int speed) {
    analogWrite(ENA, speed);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    analogWrite(ENB, speed);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnRight(int speed) {
    analogWrite(ENA, speed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    analogWrite(ENB, speed);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void stopMotors() {
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    analogWrite(ENB, 0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void loop() {
    if (radio.available()) {
        char text[32] = {0}; // Buffer to store the received message
        radio.read(&text, sizeof(text));

        lastMessageTime = millis(); // Reset timeout
        Serial.print("Received: ");
        Serial.println(text);

        if (strcmp(text, "STRAIGHT") == 0) {
            stopMotors();
        } else if (strcmp(text, "UP") == 0) {
            
            moveForward(255);
        } else if (strcmp(text, "LEFT") == 0) {
            
            turnLeft(255);
        } else if (strcmp(text, "RIGHT") == 0) {
            
            turnRight(255);
        } else if (strcmp(text, "DOWN") == 0) {
            
            moveBackward(255);
        } else {
            moveForward(10);
        }
    }

    // Timeout logic: Stop the robot if no signal is received for a set duration
    if (millis() - lastMessageTime > timeoutDuration) {
        stopMotors();
        Serial.println("No signal, stopping...");
    }
}
