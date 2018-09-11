#include <Arduino.h>
#include <stdint.h>

class TMC5041{
	public:
		TMC5041();
		TMC5041(uint8_t CSpin);
		TMC5041(uint8_t CSpin, uint8_t pot1pin, uint8_t pot2pin);
		void begin();
		void changeStepsPerRevolution(uint8_t motor, int steps);
		void moveSteps(uint8_t motor, long location);
		void moveAngle(uint8_t motor, float angle);
		void sendData(unsigned long address, unsigned long datagram);
		void correctLocation();
		int getLocation(uint8_t motor);
		unsigned long readData(unsigned long address);
		uint8_t reachedTarget(uint8_t motor);
		uint8_t stalled(uint8_t motor);
		unsigned long stallGuardResult(uint8_t motor);
	
	private:
		uint8_t chipCS = 8;
		uint8_t enable = 7;
		int motor1StepsPerRevolution = 400;
 		int motor2StepsPerRevolution = 400;
 		int uSteps = 51200;
 		uint8_t potentiometer1pin;
 		uint8_t potentiometer2pin;
 		unsigned int ADCresolution = 4095;  // highest value your ADC can return, if using potentiometers
};