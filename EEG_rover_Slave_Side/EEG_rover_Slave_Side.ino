//Code for slave: 
#include <SoftwareSerial.h>

// HC-05 Slave on pins 10 (RX), 11 (TX)
SoftwareSerial btSerial(10, 11); // RX, TX

// Motor driver pins
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9
#define ENA 5
#define ENB 3

String incoming = "";
int attention = 0;
const int threshold = 50;

void setup() {
  Serial.begin(115200);        // USB Serial Monitor
  btSerial.begin(38400);       // HC-05 baud rate

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopMotors();
  Serial.println("Slave Ready. Waiting for data...");
}

void loop() {
  while (btSerial.available()) {
    char c = btSerial.read();
    Serial.print(c);  // Debug each char received
    if (c == '\n') {
      Serial.println(); // Newline for readability
      Serial.print("Full message received: ");
      Serial.println(incoming);

      if (incoming.startsWith("ATT:")) {
        attention = incoming.substring(4).toInt();
        Serial.print("Parsed Attention: ");
        Serial.println(attention);

        if (attention > threshold) {
          int speed = map(attention, threshold + 1, 100, 100, 255);
          Serial.print("Mapped Speed: ");
          Serial.println(speed);
          moveForward(speed);
        } else {
          Serial.println("Below threshold. Stopping motors.");
          stopMotors();
        }
      } else {
        Serial.println("Invalid message format.");
      }
      incoming = "";
    } else {
      incoming += c; // Append character to incoming message
    }
  }
}

void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  Serial.println("Motors running forward.");
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println("Motors stopped.");
}