#include <Arduino.h>
#include <math.h>
#include "MovementControl.h"
#include "settings.h"

MovementController *mc; 

bool state = false; 

void setup(){
  mc = new MovementController(880, 480, 100);

  // activate serial connection
  Serial.begin(115200);
  while (!Serial) { 
    ; // wait for serial port to connect. Needed for native USB
  }

  pinMode(SW_PIN, OUTPUT);
}


void loop(){
  if (digitalRead(SW_PIN)) {
    state = true;
    delay(1000);
  }
  if (state) {
    state = false;
    delay(2000);

    mc->move_to(400, 400, 400);
  }
}
