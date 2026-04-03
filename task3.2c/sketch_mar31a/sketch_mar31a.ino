#include <WiFiNINA.h>
#include <PubSubClient.h>

#define TRIG 5
#define ECHO 6

#define LED1 3
#define LED2 7

char ssid[] = "Joyiphone";
char pass[] = "123456778";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* broker = "broker.emqx.io";

long getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration * 0.034 / 2;

  return distance;
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String message = "";

  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Message received on ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == "ES/Wave")
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
  }
  else if (String(topic) == "ES/Pat")
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("FarahClient123"))
    {
      Serial.println("MQTT connected");
      client.subscribe("ES/Wave");
      client.subscribe("ES/Pat");
    }
    else
    {
      Serial.print("MQTT failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Still connecting to WiFi...");
  }

  Serial.println("WiFi connected");

  client.setServer(broker, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  long distance = getDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance < 5)
  {
    client.publish("ES/Pat", "Farah");
    Serial.println("Pat detected - LEDs OFF");
    delay(2000);
  }
  else if (distance >= 5 && distance < 15)
  {
    client.publish("ES/Wave", "Farah");
    Serial.println("Wave detected - LEDs ON");
    delay(2000);
  }

  delay(200);
}