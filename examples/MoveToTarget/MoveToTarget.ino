#include "TMC5041.h"

TMC5041 motorGroup1 = TMC5041(D5); // D5 is the chipselect pin I'm using on the Electron
//TMC5041 motorGroup1 = TMC5041(8); // 8 is the chipselect pin for this SPI device
// TMC5041 motorGroup1 = TMC5041(8,pot1pin,pot2pin);

int potValue = 0;

void setup(){
  Serial.begin(9600);
	motorGroup1.begin();
//   pinMode(13, OUTPUT);
//   pinMode(A0, INPUT);

  //motorGroup1.changeStepsPerRevolution(0,8000); 
  
}

void loop(){
  // The '0' parameter corresponds to sending the same command for both motors
	motorGroup1.moveAngle(0, 2000);
	while(!motorGroup1.reachedTarget(1)){
    while(!motorGroup1.reachedTarget(2)){
      delay(100);
	  }
	}
  
  delay(1000);
  Serial.print(motorGroup1.getLocation(1)); // Returns location in steps, based on the potentiometer rather than on the controller's internal count
  Serial.print(", ");
  Serial.println(motorGroup1.getLocation(2));
  
	motorGroup1.moveAngle(0, 1);
  while(!motorGroup1.reachedTarget(1)){
    while(!motorGroup1.reachedTarget(2)){
      delay(100);
    }
  }

  //digitalWrite(13,LOW);
}