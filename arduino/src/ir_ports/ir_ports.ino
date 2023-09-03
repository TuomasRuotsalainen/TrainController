#include <Railuino.h>

const word    LOCO  = ADDR_MM2 + 45;
const word    LOW_SPEED = 200;
const word    HIGH_SPEED = 400;
const word    TIME  = 5000;
const boolean DEBUG = true;

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

void setup() {

    irPort1.pin = irPort1Pin;
    irPort1.openValue = analogRead(irPort1.pin);
    irPort1.isClosed = false;

   Serial.begin(9600);  // Start the Serial communication for debugging
    Serial.println("Set openvalue for ir port 1 to " + String(irPort1.openValue));

    //Serial.begin(115200);
    //while (!Serial);
  
    ctrl.begin();

    ctrl.setPower(true);
    ctrl.setLocoFunction(LOCO, 0, 1);
    ctrl.setLocoDirection(LOCO, DIR_REVERSE);
    ctrl.setLocoSpeed(LOCO, LOW_SPEED);



}

void loop() {
    lightValue = analogRead(irPort1.pin);

    if (lightValue < irPort1.openValue * portCloseThreshold) {
        if (irPort1.isClosed == false) {
          Serial.println("Port 1 is closed");
          ctrl.setLocoDirection(LOCO, DIR_FORWARD);
          ctrl.setLocoSpeed(LOCO, LOW_SPEED);
          delay(TIME);
          ctrl.setLocoDirection(LOCO, DIR_REVERSE);
          ctrl.setLocoSpeed(LOCO, HIGH_SPEED);
        }
        irPort1.isClosed = true;
    } else {
        if (irPort1.isClosed == true) {
            Serial.println("Port 1 is opened");
        }
        irPort1.isClosed = false;
    }



    //Serial.println(lightValue);

    delay(100);

}