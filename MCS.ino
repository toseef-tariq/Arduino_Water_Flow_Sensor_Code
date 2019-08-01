#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>

// Wifi and SERVER Setting
const char* ssid     = "The Doctor Design";
const char* password = "3156725415";
char servername[]="evolve.tk";
WiFiClient client;

// water Flow Sensor Settings
byte statusLed    = 13;
byte sensorInterrupt = 0;
byte sensorPin       = 8;
float calibrationFactor = 4.5;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;
int count = 0;
void setup(){
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("\nWiFi connected");
  
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);
  
  pinMode(D8, INPUT);
  digitalWrite(D8, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop()
{
   if((millis() - oldTime) > 1000) {
    detachInterrupt(sensorInterrupt);
    count++;
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));
    Serial.print(".");
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC);
    Serial.print("L/min");
    Serial.print("  Current Liquid Flowing: ");
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    Serial.print("  Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 
    pulseCount = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
  if(count == 5){
    sendGET(totalMilliLitres);
    totalMilliLitres = 0;
    count = 0;
  }
}
void sendGET(int totalMilliLitres) //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    Serial.println("connected");
    String url = "/test.php?total="+(String) totalMilliLitres;
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: evolve.tk\r\n" + 
               "Connection: close\r\n\r\n");  
    client.println(); //end of get request
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }
  client.stop(); //stop client
}


/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
