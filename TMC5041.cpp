#include "TMC5041.h"

TMC5041::TMC5041(){ bool trashes = false;}
TMC5041::TMC5041(uint8_t CSpin) { chipCS = CSpin; }
TMC5041::TMC5041(uint8_t CSpin, uint8_t pot1pin, uint8_t pot2pin) { 
	chipCS = CSpin;
	potentiometer1pin = pot1pin;
	potentiometer2pin = pot2pin; 
}

void TMC5041::begin() {
	pinMode(chipCS,OUTPUT);
 	pinMode(enable, OUTPUT);
 	digitalWrite(chipCS,HIGH);
 	digitalWrite(enable,LOW);

 	SPI.setBitOrder(MSBFIRST);
 	SPI.setClockDivider(SPI_CLOCK_DIV8);
 	SPI.setDataMode(SPI_MODE3);
 	SPI.begin();

 	sendData(0x80,0x00000000);      //GCONF

 	sendData(0xA0,0x00000000);     //RAMPMODE=0
  	sendData(0xC0,0x00000000);     //RAMPMODE=0

  	// Motor 1
  	sendData(0xA3,0x0000000F);     // VSTART = 15
 	sendData(0xA4,0x000003E8);     //A1=1000
 	sendData(0xA5,0x000186A0);     //V1=100000
 	sendData(0xA6,0x0000C350);     //AMAX=50000
 	sendData(0xA7,0x000186A0);     //VMAX=100000
 	sendData(0xA8,0x000002BC);     //DMAX
 	sendData(0xAA,0x00000578);     //D1=1400
 	sendData(0xAB,0x00000020);     //VSTOP=32
 	sendData(0xAC,0x0000000A);      //TPOWERDOWN=10
  
  	// Motor 2
	sendData(0xC3,0x0000000F);     // VSTART = 15
  	sendData(0xC4,0x000003E8);     //A1=1000
 	sendData(0xC5,0x000186A0);     //V1=100000
 	sendData(0xC6,0x0000C350);     //AMAX=50000
 	sendData(0xC7,0x000186A0);     //VMAX=100000
 	sendData(0xC8,0x000002BC);     //DMAX
 	sendData(0xCA,0x00000578);     //D1=1400
 	sendData(0xCB,0x00000020);     //VSTOP=32
 	sendData(0xCC,0x0000000A);      //TPOWERDOWN=10

  
 	//IHOLD Config                                            delay  irun   ihold
 	// unsigned long iholdconfig = 0b 0000 0000 0000 0000 00   0011  11110  00000;
 	unsigned long iholdconfig = 0b00000000000000000000111111000000;
 	sendData(0xB0, iholdconfig);
 	sendData(0xD0, iholdconfig);


	// was: sendData(0xD0,0x00000805);      //IHOLD_IRUN: IHOLD=3, IRUN=10 (max.current), IHOLDDELAY=6
	//    : sendData(0xB0,0x00000805);      //IHOLD_IRUN: IHOLD=3, IRUN=10 (max.current), IHOLDDELAY=6

  //CHOPPER Config
  //sendData(0xEC,0x000101D5);      //CHOPCONF: TOFF=5, HSTRT=5, HEND=3, TBL=2, CHM=0 (spreadcycle)
  unsigned long chopconfig = 0b00000000000000001000000000000010;
  sendData(0xEC,chopconfig);
  sendData(0xFC,chopconfig);
  
  // PWM Config                                freewheel    autoscale   pwm_freq    pwm_grad   pwm_amplitude        
  //unsigned long pwmconfig = 0b 0000 0000  00     01    0      1          01       11111111   11111111;
  unsigned long pwmconfig = 0b00000000001001011111111111111111; 
  sendData(0x90,pwmconfig);      // PWMCONF Motor1
  sendData(0x98,pwmconfig);      // PWMCONF Motor2

    

  // cool config 00000000000000000001101111 0x6D
  //sendData(0xEC,0b0000 0000 0000 00001 000 0 0000 000 0010 00 0000 0000 0000 0000 0110 1111);
  //sendData(0x7C,0x000101D5);      //CHOPCONF: TOFF=5, HSTRT=5, HEND=3, TBL=2, CHM=0 (spreadcycle)


  // add in zeroing against limit switch?

  // Set up origins
  //correctLocation();
  sendData(0xA1,0x00000000);
  sendData(0xA1,0x00000000);
  sendData(0xAD,0x00000000);     //XTARGET=0
  sendData(0xCD,0x00000000);     //XTARGET=0


}

void TMC5041::changeStepsPerRevolution(uint8_t motor, int steps){
	// Common step values will be 400 (0.9 degree resolution), 200 (1.8 degree resolution)

	switch(motor){
		case 0:       // change both, probably the most common use
			motor1StepsPerRevolution = steps;
			motor2StepsPerRevolution = steps;
			break;
		case 1: 
			motor1StepsPerRevolution = steps;
			break;
		case 2: 
			motor2StepsPerRevolution = steps;
			break;
	}

	// Terrible no-good very bad code on the next line!!!!
	uSteps = steps*256;
}

void TMC5041::moveAngle(uint8_t motor, float angle){
	//correctLocation();
	long microsteps = uSteps*angle/360;    // assumes 256 microsteps
	switch(motor){
		case 0:
			sendData(0xAD, microsteps);   //0xAD = 0x2D + 0x80 for write access
			sendData(0xCD, microsteps);   //0xCD = 0x4D + 0x80 for write access
			break;
		case 1: 
			sendData(0xAD, microsteps);   //0xAD = 0x2D + 0x80 for write access
			break;
		case 2: 
			sendData(0xCD, microsteps);   //0xCD = 0x4D + 0x80 for write access
			break;
	}
}

void TMC5041::moveSteps(uint8_t motor, long steps){
	//correctLocation();
	long microsteps = steps*uSteps; 
	switch(motor){
		case 0:
			sendData(0xAD, microsteps);   //0xAD = 0x2D + 0x80 for write access
			sendData(0xCD, microsteps);   //0xCD = 0x4D + 0x80 for write access
			break;
		case 1: 
			sendData(0xAD, microsteps);   //0xAD = 0x2D + 0x80 for write access
			break;
		case 2: 
			sendData(0xCD, microsteps);   //0xCD = 0x4D + 0x80 for write access
			break;
	}
}

int TMC5041::getLocation(uint8_t motor){
	int realLocation = 0;
	int potValue = 0;
	switch(motor){
		case 1:
			potValue = analogRead(potentiometer1pin);
		 	delay(10);
		 	potValue += analogRead(potentiometer1pin);
		 	delay(10);
		 	potValue += analogRead(potentiometer1pin);
		 	potValue /= 3;
			realLocation = map(potValue,0,ADCresolution,1,motor1StepsPerRevolution);
			break;
		case 2:
			potValue = analogRead(potentiometer2pin);
		 	delay(10);
		 	potValue += analogRead(potentiometer2pin);
		 	delay(10);
		 	potValue += analogRead(potentiometer2pin);
		 	potValue /= 3;
			realLocation = map(potValue,0,ADCresolution,1,motor2StepsPerRevolution);
			break;
	}
	 
	return realLocation;
}

void TMC5041::sendData(unsigned long address, unsigned long datagram) {
  //TMC5xxx takes 40 bit data: 8 address and 32 data

  //delay(100);                  //<------ just commented this out, if there's a problem
  unsigned long i_datagram;

  digitalWrite(chipCS,LOW);
  delayMicroseconds(10);

  SPI.transfer(address); 

  i_datagram |= SPI.transfer((datagram >> 24) & 0xFF);
  i_datagram <<= 8;
  i_datagram |= SPI.transfer((datagram >> 16) & 0xFF);
  i_datagram <<= 8;
  i_datagram |= SPI.transfer((datagram >> 8) & 0xFF);
  i_datagram <<= 8;
  i_datagram |= SPI.transfer((datagram) & 0xFF);
  digitalWrite(chipCS,HIGH);

  // Serial.print("Received: ");
  // Serial.println(i_datagram,HEX);
  // Serial.print(" from register: ");
  // Serial.println(address,HEX);
}

unsigned long TMC5041::readData(unsigned long address){
	sendData(address, 0x00000000); // send address, clock in some junk, clear previous junk
	delayMicroseconds(10);

	unsigned long i_datagram; // storage for stuff

 	digitalWrite(chipCS,LOW);
 	delayMicroseconds(10);

 	SPI.transfer(address); // might as well hit that address again, though just queues up junk for the next read/write

 	i_datagram |= SPI.transfer(0x00);
 	i_datagram <<= 8;
 	i_datagram |= SPI.transfer(0x00);
 	i_datagram <<= 8;
 	i_datagram |= SPI.transfer(0x00);
 	i_datagram <<= 8;
 	i_datagram |= SPI.transfer(0x00);
 	digitalWrite(chipCS,HIGH);

 	return i_datagram;
}

uint8_t TMC5041::reachedTarget(uint8_t motor){
	unsigned long address = 0x00;
	if(motor == 1){
		address = 0x35;
	}
	else if(motor == 2){
		address = 0x55;
	}

	unsigned long rampStat = readData(address);

	// bitmask out the values we want. position_reached is bit9, event_pos_reached is bit7 (this one clears on read), 
	// event_stop_sg is bit6 and indicates a stallguard event- will clear, and motor may start moving again!
	// 0b00000000 00000000 00000000 10000000 == bit7

	unsigned long bitmask = 0b00000000000000000000000010000000;
	unsigned long bitmaskResult = rampStat & bitmask;

	if(bitmaskResult == bitmask){ return 1; }
	else { return 0; }
}

void TMC5041::correctLocation(){   // As written, will only work with 360 degree max range!
	
	// Converts a 0-ADCmax range to a 1-max steps in a revolution range
	int potValue = analogRead(potentiometer1pin);
 	delay(10);
 	potValue += analogRead(potentiometer1pin);
 	delay(10);
 	potValue += analogRead(potentiometer1pin);
 	potValue /= 3;

	int realLocation = map(potValue,0,ADCresolution,1,motor1StepsPerRevolution);
	sendData(0xA1,realLocation);


	potValue = analogRead(potentiometer2pin);
 	delay(10);
 	potValue += analogRead(potentiometer2pin);
 	delay(10);
 	potValue += analogRead(potentiometer2pin);
 	potValue /= 3;

	realLocation = map(potValue,0,ADCresolution,1,motor2StepsPerRevolution);
	sendData(0xC1,realLocation);
}


// Not currently working, probably requires different register configuration to be in the right mode
uint8_t TMC5041::stalled(uint8_t motor){
	unsigned long address = 0x00;
	if(motor == 1){
		address = 0x35;
	}
	else if(motor == 2){
		address = 0x55;
	}

	unsigned long rampStat = readData(address);

	// bitmask out the values we want. position_reached is bit9, event_pos_reached is bit7 (this one clears on read), 
	// event_stop_sg is bit6 and indicates a stallguard event- will clear, and motor may start moving again!
	// 0b00000000 00000000 00000000 01000000 == bit7

	unsigned long bitmask = 0b00000000000000000000000001000000;
	unsigned long bitmaskResult = rampStat & bitmask;

	if(bitmaskResult == bitmask){ return 1; }
	else { return 0; }
}

unsigned long TMC5041::stallGuardResult(uint8_t motor){
	unsigned long address = 0x00;
	if(motor == 1){
		address = 0x35;
	}
	else if(motor == 2){
		address = 0x55;
	}

	unsigned long rampStat = readData(address);

	// bitmask out the values we want. position_reached is bit9, event_pos_reached is bit7 (this one clears on read), 
	// event_stop_sg is bit6 and indicates a stallguard event- will clear, and motor may start moving again!
	// 0b00000000 00000000 00000000 01000000 == bit7

	unsigned long bitmask = 0b00000000000000000000000011110000;
	unsigned long bitmaskResult = rampStat & bitmask;

	return bitmaskResult;
}
