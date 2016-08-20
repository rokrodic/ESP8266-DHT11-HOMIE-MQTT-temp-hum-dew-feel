//Dependencies: Homie and Adafruit-DHT Library.

#define FW_NAME "dht-node"
#define FW_VERSION "1.0.6"
//#define Sensor_DS1820 1
//#define Sensor_DHT11  1

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";

#include <DHT.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>

#define DHTTYPE     DHT11 //DHT11, DHT21, DHT22
#define DHTPIN      2
//#define DS1820PIN   5

#include <Homie.h>

const int TEMPERATURE_INTERVAL = 30; //seconds between reads/sends

unsigned long lastTemperatureSent = 0;
//float temperature = 0; // Fake temperature here, for the example

// Initialize DHT sensor
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01 
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266
 
//float hum, temp; //, tempDS1820;  // Values read from sensor
//unsigned long previousMillis = 0; //, previousMillisDS1820 = 0;        // will store last temp was read
//const long interval = 2000;              // interval at which to read sensor

//OneWire oneWire(DS1820PIN);
//DallasTemperature sensors(&oneWire);

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
    //temperature += 0.1; // Fake temperature here, for the example
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
    //if (Homie.setNodeProperty(temperatureNode, "degrees", String(temperature), 1, true)) {
    lastTemperatureSent = millis();
    //} else {
    //  Serial.println("Temperature sending failed");
    //}
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  //sensors.begin();
  Homie_setFirmware(FW_NAME, FW_VERSION);
  //Homie.registerNode(temperatureNode);
  //Homie.registerNode(heatindexNode);
  //Homie.registerNode(humidityNode);
  Homie.setSetupFunction(setupHandler);
  //Homie.setResetFunction(resetFunction);
  Homie.setLoopFunction(loopHandler);
  Homie.setup();
}

//void gettemperatureDS1820() {
//  unsigned long currentMillis = millis();
//  if(currentMillis - previousMillisDS1820 >= interval) {
//    previousMillisDS1820 = currentMillis;   
//    // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
//    sensors.requestTemperatures(); // Send the command to get temperatures
//    // After we got the temperatures, we can print them here. We use the function ByIndex, and as an example get the temperature from the first sensor only.
//    // Temperature for the device 1 (index 0) is
//    tempDS1820 = sensors.getTempCByIndex(0);
//    if (isnan(tempDS1820)) {
//      Serial.println(F("Failed to read from DS1820 sensor!"));
//      return;
//    }
//  }
//}

void loop() {
  Homie.loop();
}
