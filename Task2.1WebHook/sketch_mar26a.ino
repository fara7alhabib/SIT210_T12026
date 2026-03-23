#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <DHT.h>

// WiFi
char ssid[] = "Joyiphone";
char pass[] = "1234567788";

// ThingSpeak
unsigned long channelID = 3308237;
const char *writeAPIKey = "QAEUME962XEWO4HS";

// DHT22
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Light sensor
int lightPin = A0;

// WiFi client
WiFiClient client;

// Timing
unsigned long lastTime = 0;
unsigned long delayTime = 15000;   // 15 seconds

void setup()
{
  Serial.begin(9600);
  delay(3000);

  Serial.println("Starting system...");

  dht.begin();

  connectWiFi();
  ThingSpeak.begin(client);

  Serial.println("Setup done.");
}

void loop()
{
  if (millis() - lastTime > delayTime)
  {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int light = analogRead(lightPin);

    // Print readings
    Serial.print("Temperature: ");
    Serial.println(temp);

    Serial.print("Humidity: ");
    Serial.println(hum);

    Serial.print("Light: ");
    Serial.println(light);

    // Send data to ThingSpeak
    if (!isnan(temp))
    {
      ThingSpeak.setField(1, temp);
    }

    if (!isnan(light))
    {
      ThingSpeak.setField(2, light);
    }

    int result = ThingSpeak.writeFields(channelID, writeAPIKey);

    Serial.print("ThingSpeak result: ");
    Serial.println(result);
    Serial.println("----------------------");

    lastTime = millis();
  }
}

void connectWiFi()
{
  Serial.print("Connecting to WiFi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.print(".");
    delay(3000);
  }

  Serial.println();
  Serial.println("WiFi connected.");
}