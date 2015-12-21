/*
  WaterCountAndBoost

  End point for Water meter counters and water temp realy. control inteligence
  provided by Node-Red Home automation set up

  Water Count:
  Reads switch closures form 2 pulse water meters (Rain Water & Mains Water)
  Has variable with thingspeak update string
  Has Function to clear current count on request

  Heat Boost:
  Has function to take new set temperature, when water reaches set temp, resets
  to Frost protect value.

*/

#include "ThingSpeak.h"

TCPClient client;

//Boost Temp Variables
int setPointInternal = 10;
bool callForHeat = false;

int heatCallCounter = 0;
int stopHeatCounter = 0;

int lastUpdate = Time.now();
const int updateInterval = 5;

//reads solar data channel
unsigned long hotWaterChannelNumber = 22064;
const char * myReadAPIKey = "U3OI8EDGNDTPLA89";

//Water Counter Variables
#define ARRAY_SIZE 2
// array of pins to be debounced
short pinsToDebounce[]={
  0,2
};
// array of pin state
int swStates[]={
  0,0};
// array of previous pin state
int swPrevStates[]={
  0,0};
// array to store the actual state during debounce
int swDebouncedStates[]={
  0,0};
// array to store the previous state during debounce
int swPrevDebounceStates[]={0,0};
// time to debounce
int debounceDelay=10;
// array of previous times the pin has been checked
long prevTimes[]={0,0};

int meterCounts[]={0,0};

char thingspeakDataPackage[20];
String theData="";

void setup() {
  // Water Counter Set up
  Particle.variable("count1", &meterCounts[0], INT);
  Particle.variable("count2", &meterCounts[1], INT);
  Particle.variable("dataPack", thingspeakDataPackage, STRING);
  Particle.function("resetCount", resetCount);
  initSwitches();

  // Water Boost Set up
    pinMode(D7, OUTPUT);
    ThingSpeak.begin(client);
    Particle.function("water", setNewWaterSetPoint);
}

void initSwitches(){
  for(int i=0;i<ARRAY_SIZE;i++){
    pinMode(pinsToDebounce[i],INPUT);
  }
}

void loop() {
  // water count loop
  readSwitches();
  theData = "field1="+String(meterCounts[0]*10)+"&field2="+String(meterCounts[1]*10);
  strcpy(thingspeakDataPackage,theData.c_str());

  // Boost Temp Loop
  if(Time.now() > (lastUpdate + updateInterval)) {

      float topTemp = ThingSpeak.readFloatField(hotWaterChannelNumber, 3, myReadAPIKey);
      if(callForHeat){
          digitalWrite(D7, LOW);
      } else {
          digitalWrite(D7, HIGH);
      }

      if (topTemp<setPointInternal){
          heatCallCounter++;
          if(heatCallCounter > 12){
              callForHeat = true;
              heatCallCounter = 0;
          }
      }

      if (topTemp>setPointInternal){
          stopHeatCounter++;
          if(stopHeatCounter > 36){
              callForHeat = false;
              stopHeatCounter = 0;
              setPointInternal = 10;
              Particle.publish("cwudum-waterTemp", "Tank Reached set Temp. Now at: " + String(topTemp) + "Deg",15,PRIVATE);
          }
      }
      lastUpdate = Time.now();
  }
}

int setNewWaterSetPoint(String command) {
    Particle.publish("cwudum-waterTemp", "Temp Set to: " + command,60,PRIVATE);
    setPointInternal = atoi(command.c_str());
    return 1;
}

void readSwitches(){
  for(short sw=0;sw<ARRAY_SIZE;sw++){
    volatile int pin=pinsToDebounce[sw];
    volatile int tempPin=pin;
    volatile int pinPosition=sw;
    swStates[pinPosition]=digitalRead(pin);
  }
   debouncePins();
   checkStateChange();
}
void debouncePins(){
  volatile long _millis=millis();
  for(short sw=0;sw<ARRAY_SIZE;sw++){
    if(swStates[sw]!=swPrevStates[sw]){
      prevTimes[sw]=_millis;
    }
    if(_millis-prevTimes[sw]>debounceDelay){
      prevTimes[sw]=_millis;
      swDebouncedStates[sw]=swStates[sw];
    }
    swPrevStates[sw]=swStates[sw];
  }
}

void checkStateChange(){
  for(short sw=0;sw<ARRAY_SIZE;sw++){
    if(swPrevDebounceStates[sw]!=swDebouncedStates[sw]){
      if(swDebouncedStates[sw]==1){
        pinActive(sw);
      }
      if(swDebouncedStates[sw]==0){
        pinInactive(sw);
      }
    }
    swPrevDebounceStates[sw]=swDebouncedStates[sw];
  }
}

void printDebStates(){
  for(int i=0;i<ARRAY_SIZE;i++){
  };
}
void pinActive(int _id){
  meterCounts[_id]++;
}

void pinInactive(int _id){
 // no code for inactive state
}

int resetCount(String command)
{
  int returnValue = -1;
  for(short sw=0;sw<ARRAY_SIZE;sw++){
    meterCounts[sw] = 0;
  }
  returnValue = 1;

  return returnValue;
}
