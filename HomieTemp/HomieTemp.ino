//Dependencies: Homie and Adafruit-DHT Library.

#define FW_NAME "dht-node"
#define FW_VERSION "1.0.6"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";

#include <DHT.h>
#include <Homie.h>

#define DHTTYPE     DHT11 //DHT11, DHT21, DHT22
#define DHTPIN      2

const int TEMPERATURE_INTERVAL = 30; //seconds between reads/sends
unsigned long lastTemperatureSent = 0;

// Initialize DHT sensor
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01 
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
 
HomieNode temperatureNode("temperature", "temperature");
HomieNode heatindexNode("heatindex", "heatindex");
HomieNode dewpointNode("dew", "dew");
HomieNode humidityNode("humidity", "humidity");

void setupHandler() {
  dht.begin();
  Homie.setNodeProperty(temperatureNode, "unit", "c", true);
  Homie.setNodeProperty(heatindexNode, "unit", "c", true);
  Homie.setNodeProperty(dewpointNode, "unit", "c", true);
  Homie.setNodeProperty(humidityNode, "unit", "%", true);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
float dewPointFast(float celsius, float humidity) {
  float a = 17.271;
  float b = 237.7;
  float temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
  float Td = (b * temp) / (a - temp);
  return Td;
}

void loopHandler() {
  if (millis() < lastTemperatureSent) lastTemperatureSent = 0; //Correct the 29day overflow
  if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.print("Invalid DHT data.");
      return;
    }
    
    float hic = dht.computeHeatIndex(temperature, humidity, false);

    float dew = dewPointFast(temperature, humidity);
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Temp.index: ");
    Serial.print(hic);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Dew: ");
    Serial.print(dew);
    Serial.println(" °C");
    
    Homie.setNodeProperty(temperatureNode, "degrees", String(temperature), true);
    Homie.setNodeProperty(humidityNode, "percent", String(humidity), true);
    Homie.setNodeProperty(dewpointNode, "degrees", String(dew), true);
    Homie.setNodeProperty(heatindexNode, "degrees", String(hic), true);
    lastTemperatureSent = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
