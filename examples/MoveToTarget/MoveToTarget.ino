#include <TMC5041.h>

TMC5041 motorGroup1 = TMC5041(8);

void setup(){
	motorGroup1.begin();
}

void loop(){
	motorGroup1.moveAngle(180);
	delay(2000);
	motorGroup1.moveAngle(0);
	delay(2000);
}