//Hot Water Smart Thermostat
int setPointInternal = 0;
bool heatCalled = false;

void setup()
{
  Serial.begin(9600);

    //Set up spark variables to read setPoint
    Spark.variable("setPoint", &setPointInternal, INT);

    Spark.function("alterSetPoint", resetCount);

}

void loop()
{
  if
}
