#include <Railuino.h>

const word    LOCO  = ADDR_MM2 + 45;
const word    SPEED = 50;
const word    TIME  = 5000;
const boolean DEBUG = true;

TrackController ctrl(0xdf24, DEBUG);

void setup() {
  Serial.begin(115200);
  while (!Serial);


  
  ctrl.begin();

  ctrl.setPower(true);
  ctrl.setLocoFunction(LOCO, 0, 1); 
  
}

void loop() {

  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  delay(500);                   
  digitalWrite(0, LOW); 
  delay(1000);  

  word speed;
  
  Serial.println("Go");
  ctrl.setLocoFunction(LOCO, 0, 1);
  ctrl.setLocoDirection(LOCO, DIR_FORWARD);
  ctrl.setLocoSpeed(LOCO, SPEED);
  //if (ctrl.getLocoSpeed(LOCO, &speed)) {}
  
  delay(TIME);
  
  Serial.println("Stop");
  ctrl.setLocoSpeed(LOCO, 0);

  ctrl.setLocoFunction(LOCO, 0, 0); 

  

  delay(TIME);

  ctrl.setLocoFunction(LOCO, 0, 1);
  ctrl.setLocoFunction(LOCO, 1, 1);
  ctrl.setLocoDirection(LOCO, DIR_REVERSE);
  Serial.println("Reverse");
  ctrl.setLocoSpeed(LOCO, SPEED);

    delay(TIME);

}
