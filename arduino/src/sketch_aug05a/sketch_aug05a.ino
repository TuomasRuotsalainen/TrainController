#include <Railuino.h>
const word LOCO  = ADDR_MM2 + 45;
const boolean DEBUG = false;
const word    SPEED = 150;

// Pin where the LED is connected
const int ledPin = 13; // Built-in LED on most Arduino boards

const byte CMD_BUF_SIZE = 32;
char cmdBuffer[CMD_BUF_SIZE];
byte cmdIndex = 0;

TrackController ctrl(0xdf24, DEBUG);

const int irPort1Pin = A1;
int irPort1OpenValue = 0;
float portCloseThreshold = 0.7;

struct IRPort {
    int pin;  // pin where the photoresistor is connected
    int openValue;
    bool isClosed;
};

IRPort irPort1;

int lightValue = 0;

unsigned long lastIrPortChangeTime = 0;
bool lastIrPortReadingClosed = false;

int portCloseTimeMillis = 100;

void reverse() {
  ctrl.setLocoDirection(LOCO, DIR_REVERSE);
  ctrl.setLocoSpeed(LOCO, SPEED);
}

void ahead() {
  ctrl.setLocoDirection(LOCO, DIR_FORWARD);
  ctrl.setLocoSpeed(LOCO, SPEED);
}

void stop() {
  ctrl.setLocoSpeed(LOCO, 0);
}

void setup() {
  Serial.begin(9600);
  Serial.println("SETUP");
  
  ctrl.begin();
  ctrl.setPower(true);
  ctrl.setLocoFunction(LOCO, 0, 1);

  ahead();

  
  pinMode(ledPin, OUTPUT); // Set LED pin as an output

  irPort1.pin = irPort1Pin;
  irPort1.openValue = analogRead(irPort1.pin);
  irPort1.isClosed = false;
}

void loop() {
  lightValue = analogRead(irPort1.pin);
  bool currentIrPortReadingClosed = (lightValue < irPort1.openValue * portCloseThreshold);

  if (currentIrPortReadingClosed  != lastIrPortReadingClosed) {
        // State changed — reset timer
        lastIrPortChangeTime = millis();
        lastIrPortReadingClosed = currentIrPortReadingClosed ;
    }

    // If state stayed the same for at least portCloseTimeMillis, commit the change
    if ((millis() - lastIrPortChangeTime) > portCloseTimeMillis && irPort1.isClosed != currentIrPortReadingClosed) {
        irPort1.isClosed = currentIrPortReadingClosed;
        if (currentIrPortReadingClosed) {
          Serial.println("PORT1-CLOSED");
        } else {
          Serial.println("PORT1-OPEN");
        }
    }

    /*

    if (lightValue < irPort1.openValue * portCloseThreshold) {
        if (irPort1.isClosed == false) {
          Serial.println("Port 1 is closed");
        }
        irPort1.isClosed = true;
    } else {
        if (irPort1.isClosed == true) {
            Serial.println("Port 1 is opened");
        }
        irPort1.isClosed = false;
    }*/
    
  
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
    ctrl.setLocoFunction(LOCO, 0, 1);
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
  else if (strcmp(command, "REVERSE") == 0) {
    Serial.println("REVERSE_OK");
    ctrl.setLocoFunction(LOCO, 0, 1);
    reverse();
  } else if (strcmp(command, "AHEAD") == 0) {
    ahead();
  } else if (strcmp(command, "STOP") == 0) {
    Serial.println("STOP_OK");
    stop();
  } else {
    Serial.print("Unknown command: ");
    Serial.println(command);
  }
}
