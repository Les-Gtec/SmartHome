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
int debounceDelay=250;
// array of previous times the pin has been checked
long prevTimes[]={
  0,0};

int meterCounts[]={0,0};

bool firstPass = true;

// Thinkspeak channel information
String writeAPIKey = "JBMCE6T3E4RP23OI";
String channelID = "22096";

// TCP socket initialize
TCPClient client;

void setup()
{
  Serial.begin(9600);

    //Set up spark variables to read counter
    Spark.variable("count1", &meterCounts[0], INT);
    Spark.variable("count2", &meterCounts[1], INT);
    //Spark.variable("count3", &meterCounts[2], INT);
    Spark.function("pushCounts", pushCounts);
    initSwitches();
}


void initSwitches(){
  for(int i=0;i<ARRAY_SIZE;i++){
    pinMode(pinsToDebounce[i],INPUT);
  }
}

void loop()
{
  readSwitches();
  if(firstPass){
    firstPass = false;
  }
}

void readSwitches(){

  // Serial.print("active switch set ");
  // Serial.println((int)activeSwitchSet);

  for(short sw=0;sw<ARRAY_SIZE;sw++){
    volatile int pin=pinsToDebounce[sw];
    volatile int mpPin=pin;
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
      /*
      Serial.print("button ");
       Serial.print(sw);
       Serial.print(" is ");
       Serial.println(swDebouncedStates[sw]);
       */
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

  if(!firstPass){
    meterCounts[_id]++;
  }
}

void pinInactive(int _id){

}

int pushCounts(String command)
{
    //for(short sw=0;sw<ARRAY_SIZE;sw++){
    //  meterCounts[sw] = (meterCounts[sw]-1)*10;
    //}
     ThingSpeakUpdate("field1="+String(meterCounts[0])+"&field2="+String(meterCounts[1])+"&field3=0");
     //for(short sw=0;sw<ARRAY_SIZE;sw++){
//       meterCounts[sw] = 1;
     //}
     return 200;
}

/*------------------------------------------------
Sends Meter Counts to Thingspeak
Inputs: String, data to be entered for each field
Returns:
------------------------------------------------*/
void ThingSpeakUpdate(String tsData)
{
      // Connecting and sending data to Thingspeak
    if(client.connect("api.thingspeak.com", 80))
    {
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(tsData.length());
        client.print("\n\n");

        client.println(tsData); //the ""ln" is important here.

        // This delay is pivitol without it the TCP client will often close before the data is fully sent
        delay(200);
    }
    else{
        // Failed to connect to Thingspeak
    }

    if(!client.connected()){
        client.stop();
    }
    client.flush();
    client.stop();
}
