#include <Wire.h>
#include <string.h>
//#include <Memory.h>   //for "smart pointers"
#include "RepeaterEnergySensor.h"

using namespace std;

//We'll keep a single instance of our struct around and replace it's values every time
//the pi will keep a timestamped log of values in a memory-mapped file on the filesystem
struct MEASUREMENT_SAMPLE {
  float MeasuredVoltage;
  float MeasuredCurrent;
} Sample;

MEASUREMENT_SAMPLE sample;

//this is the method where we'll send whatever our sample is back over to the pi
void requestEvent(){
  //Serialize our measurement to json
  String json = "{ \"MeasuredVoltage\":\"" + String(sample.MeasuredVoltage) + "\", \"MeasuredCurrent\":\"" + String(sample.MeasuredCurrent) + "\" } ";
  
  String rpiCommand = Wire.readString();

  if(rpiCommand.equals("healthcheck")){
    Wire.write("I am alive.");
  }
  else{
    //send it to the pi
    Wire.write(convertToConstCharArray(json));
  }
}

char* convertToConstCharArray(String someString){
  int someStringLength = someString.length();
  char constCharBuffer[someStringLength + 1];
  strcpy(constCharBuffer, someString.c_str());
  return constCharBuffer;
}


//returns value in mV
float adcDecimation(int additionalSimulationBitsResolution, uint8_t pin){
  float fSamples=pow(4,(float) additionalSimulationBitsResolution);
  int samples = (int)(fSamples+0.5);
  long dv = 0;
  for(byte avg = 0;avg < 10; avg++){
    for(int j = 0; j < samples; j++){
      dv+= analogRead(pin);
    }
  }
  dv=(dv/10);
  dv = dv>>additionalSimulationBitsResolution;

  return 250 * (dv/(2 << 16));
}

void setup() {

  //only if we're in debug mode do we worry about outputting Serial.print lines
  if(DEBUG_MODE)
  {
    Serial.begin(115200);
    Serial.println("Measuring voltage...");
  }

  Wire.begin(I2C_Address);  //join our i2c bus with pre-agreed on address

  //note that the type that this expects as input, void (*)() is a method with void return type, which is defined above
  Wire.onRequest(requestEvent);
}

float readVoltageFromPin(uint8_t pin){

  if(DEBUG_MODE)
  {
    int analogReadResult = analogRead(pin);  //10 bits of accuracy in our ADC-> 2^10 = 1024
    Serial.print("Analog Result: ");  Serial.println(analogReadResult);
  }

  float convertedAnalog = (analogRead(pin) * CONVERSION_FACTOR) / 1024.0;   //ADC has 10 bit precision, 2^10 = 1024
  float input_voltage = convertedAnalog / ( VOLTMETER_R2 / (VOLTMETER_R1 + VOLTMETER_R2 ) );

  if(input_voltage < 0.1){
    input_voltage = 0.0f;
  }

  if(DEBUG_MODE){
    Serial.print("Voltage = ");
    Serial.print(input_voltage);
    Serial.println("V");
  }

  return input_voltage;
}

float readCurrentFromPins(uint8_t pin1, uint8_t pin2){
  float v1 = (analogRead(pin1) * CONVERSION_FACTOR) / 1024.0;
  float v2 = (analogRead(pin2) * CONVERSION_FACTOR) / 1024.0;

  float current = abs(v1 - v2) / SHUNT_RESISTANCE;

  current = adcDecimation(6, pin1);

  return current;
}

void loop() {

  sample.MeasuredVoltage = readVoltageFromPin(VOLTMETER_PIN);
  sample.MeasuredCurrent = readCurrentFromPins(CURRENT_SHUNT_PIN_1, CURRENT_SHUNT_PIN_2);

  delay(SAMPLE_DELAY_MS);
}

