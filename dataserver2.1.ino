#include <ESP8266WiFi.h>
#include "DHT.h"
#include <Servo.h> 
Servo myservo;
#define DHTTYPE DHT22
const char* ssid = "D'jamur";
const char* password = "djamur2017";

#define DHTPin D1
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
int tanah=A0;
int pompa=4;
int pos;
WiFiServer server(80);

void gerak()
{
  for(pos = 0; pos <= 120; pos +=5 ) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 120; pos>=0; pos-=5)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}
 
void setup() {
  Serial.begin(115200);
  delay(10);
  myservo.attach(14);
  pinMode(tanah,INPUT);
  delay(10);
  pinMode(pompa, OUTPUT);
  digitalWrite(pompa, HIGH);
  dht.begin();
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  int baca=analogRead(tanah);
  int lembab=map(baca,760,0,0,100);
  if(lembab>=700)
  {
    gerak();
  }
  else if(lembab<=700)
  {
   myservo.write(0); 
  }
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Match the request
  float hic = dht.computeHeatIndex(t, h, false);       
  dtostrf(hic, 6, 2, celsiusTemp);             
  float hif = dht.computeHeatIndex(f, h);
  dtostrf(hif, 6, 2, fahrenheitTemp);         
  dtostrf(h, 6, 2, humidityTemp);
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(pompa, LOW);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(pompa, HIGH);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head></head><body><h1>FLUTIA the Space Digital Farming</h1><h3>Temperature: ");
  client.println(celsiusTemp);
  client.println("\xB0\tC</h3><h3>Ground Humidity: ");
  client.println(lembab);
  client.println("%</h3><h3>Air Humidity: ");
  client.println(humidityTemp);
  client.println("%</h3><h3>");
  client.println("<br><br>");
  client.print("Pump: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
 
