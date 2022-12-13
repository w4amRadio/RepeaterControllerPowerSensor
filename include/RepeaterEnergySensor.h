#include <Arduino.h>

#ifndef REPEATER_ENERGY_SENSOR_H_
#define REPEATER_ENERGY_SENSOR_H_


//define our debugging constant for when we are hooked up to a computer and not in the field
#define DEBUG_MODE true

//Pre-arranged i2c address
#define I2C_Address 0x24  //decimal 36

//Resistor constants for our voltage divider 
#define VOLTMETER_R1 10000.0F
#define VOLTMETER_R2 100000.0F

//shunt resistance
#define SHUNT_RESISTANCE 5.0f       //1-5 Ohms probably

//define the pin where we read from our voltage divider
#define VOLTMETER_PIN A1
#define CURRENT_SHUNT_PIN_1 A2
#define CURRENT_SHUNT_PIN_2 A3

//A conversion factor (the input voltage?) to determine the actual voltage, needs to be calibrated for 9-12V
//since this is the voltage of our battery.  We have a problem in that this conversion factor should actually
//not be linear.
#define CONVERSION_FACTOR 6.0f

//how often we take samples in milliseconds
#define SAMPLE_DELAY_MS 1000

#endif