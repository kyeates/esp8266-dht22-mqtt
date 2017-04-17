
/* Started with Basic MQTT example */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <DHT.h>      // DHT library from http://github.com/adafruit/DHT-sensor-library
                      // Written by ladyada, public domain

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
//DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for ESP8266:
DHT dht(DHTPIN, DHTTYPE, 20);


long previousMillis = 0;      // Timer loop from http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
long interval = 60000;        //
const char* ssid = "....";
const char* password = "....";
const char* mqtt_server = "10.1.1.67";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char data[150];
int value = 0;


void setup(){
  Serial.begin(115200);
  Serial.println("script: dht22_mqtt");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("study")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop(){
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
     // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit
//      float f = dht.readTemperature(true);
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t)) {
          Serial.println("Failed to read from DHT sensor!");
          return;
        }
      previousMillis = currentMillis;
      String(t).toCharArray(data, 100);
      client.publish("study/temperature", data);
      String(h).toCharArray(data, 100);
      client.publish("study/humidity",data);
      
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.println(" *C ");
      Serial.print("DTH sensor read and transmitted, wating for ");
      Serial.print(interval/1000);
      Serial.println("s");
      
  }
  
}
