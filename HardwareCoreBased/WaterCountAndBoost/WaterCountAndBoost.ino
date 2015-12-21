//Hot Water Smart Thermostat
int setPointInternal = 0;
bool heatCalled = false;
long nextTimeCheckMillis = millis();

void setup()
{
  Serial.begin(9600);
  pinMode(D7, OUTPUT);

    //Set up spark variables to read setPoint
    Spark.variable("setPoint", &setPointInternal, INT);

    Spark.function("alterSetPoint", newSetPoint);

}

void loop()
{
  if(heatCalled){
    digitalWrite(D7, LOW);
  } else {
    digitalWrite(D7, HIGH);
  }



}

int newSetPoint(String command)
{
  int returnValue = -1;
  //chnage set point
  returnValue = 1;

  return returnValue;
}
