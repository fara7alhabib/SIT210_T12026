#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>

// WIFI
char ssid[] = "Joyiphone";
char pass[] = "123456778";

// MQTT
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttTopic = "terrarium/light";

// Objects
WiFiClient wifiClient;
PubSubClient client(wifiClient);
BH1750 lightMeter;

// Timing
unsigned long lastMsg = 0;
const long interval = 2000;

// Threshold
const float lightThreshold = 200.0;

// State tracking
String lastState = "";

void connectWiFi()
{
  Serial.print("Connecting to WiFi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.print(".");
    Serial.print(" status=");
    Serial.println(WiFi.status());
    delay(3000);
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT... ");

    String clientId = "Nano33IoT-";
    clientId += String(random(1000, 9999));

    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry");
      delay(3000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  Wire.begin();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println("BH1750 OK");
  }
  else
  {
    Serial.println("BH1750 ERROR");
    while (1);
  }

  connectWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }

  if (!client.connected())
  {
    connectMQTT();
  }

  client.loop();

  unsigned long now = millis();

  if (now - lastMsg > interval)
  {
    lastMsg = now;

    float lux = lightMeter.readLightLevel();

    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    String currentState;

    if (lux > lightThreshold)
    {
      currentState = "high";
    }
    else
    {
      currentState = "low";
    }

    // Only publish when state changes
    if (currentState != lastState)
    {
      String message = String(lux);
      client.publish(mqttTopic, message.c_str());

      Serial.print("State changed to: ");
      Serial.println(currentState);

      Serial.print("Sent lux value: ");
      Serial.println(message);

      lastState = currentState;
    }
    else
    {
      Serial.println("No change, nothing sent");
    }
  }
}