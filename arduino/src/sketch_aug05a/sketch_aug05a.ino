// Pin where the LED is connected
const int ledPin = 13; // Built-in LED on most Arduino boards

const byte CMD_BUF_SIZE = 32;
char cmdBuffer[CMD_BUF_SIZE];
byte cmdIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); // Set LED pin as an output
}

void loop() {
  // Read serial input byte-by-byte
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      cmdBuffer[cmdIndex] = '\0';
      processCommand(cmdBuffer);
      cmdIndex = 0;
    }
    else if (c != '\r') {
      if (cmdIndex < CMD_BUF_SIZE - 1) {
        cmdBuffer[cmdIndex++] = c;
      }
    }
  }
}

void processCommand(const char* command) {
  if (strcmp(command, "LED") == 0) {
    digitalWrite(ledPin, HIGH);
    Serial.println("ACK: LED ON");
  } 
  else if (strcmp(command, "OFF") == 0) {
    digitalWrite(ledPin, LOW);
    Serial.println("ACK: LED OFF");
  } 
  else if (strcmp(command, "PING") == 0) {
    Serial.println("PING_OK");
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(command);
  }
}
