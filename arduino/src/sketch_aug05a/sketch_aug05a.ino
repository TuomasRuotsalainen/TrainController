// Pin where the LED is connected
const int ledPin = 13; // Built-in LED on most Arduino boards

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // Set LED pin as an output
}

void loop() {
    if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read until newline

    if (command == "ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("ACK: LED ON");
    } 
    else if (command == "OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("ACK: LED OFF");
    }
  }
}