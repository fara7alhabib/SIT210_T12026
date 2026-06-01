#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

char ssid[] = "Farah";
char pass[] = "123456778";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char broker[] = "broker.emqx.io";
int port = 1883;

const int LED_PIN = 2;
const int BUZZER_PIN = 3;
const int TRIG_PIN = 4;
const int ECHO_PIN = 5;

const int PRESENT_DISTANCE = 40; // cm
const unsigned long AWAY_TIME = 10000; // 10 seconds

unsigned long lastPresentTime = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Connecting to WiFi...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.print(".");
    delay(3000);
  }

  Serial.println("\nWiFi Connected");

  Serial.println("Connecting to MQTT...");

  while (!mqttClient.connect(broker, port))
  {
    Serial.print(".");
    delay(3000);
  }

  Serial.println("\nMQTT Connected");

  lastPresentTime = millis();
}

void loop()
{
  mqttClient.poll();

  long distance = getDistance();

  String motionStatus;
  String alertStatus;
  String studyModeStatus;

  if (distance > 0 && distance <= PRESENT_DISTANCE)
  {
    lastPresentTime = millis();

    motionStatus = "Present";
    alertStatus = "No Alarm";
    studyModeStatus = "Study Mode ON";

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else
  {
    motionStatus = "No Motion";

    if (millis() - lastPresentTime >= AWAY_TIME)
    {
      alertStatus = "Alarm - User Away";
      studyModeStatus = "Study Mode OFF";

      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
    }
    else
    {
      alertStatus = "Waiting";
      studyModeStatus = "Study Mode ON";

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }

  mqttClient.beginMessage("study/motion");
  mqttClient.print(motionStatus);
  mqttClient.endMessage();

  mqttClient.beginMessage("study/alert");
  mqttClient.print(alertStatus);
  mqttClient.endMessage();

  mqttClient.beginMessage("study/mode/status");
  mqttClient.print(studyModeStatus);
  mqttClient.endMessage();

  mqttClient.beginMessage("study/distance");
  mqttClient.print(distance);
  mqttClient.endMessage();

  Serial.println("--------------------");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Motion: ");
  Serial.println(motionStatus);
  Serial.print("Alert: ");
  Serial.println(alertStatus);
  Serial.print("Study Mode: ");
  Serial.println(studyModeStatus);

  delay(1000);
}

long getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
  {
    return 999;
  }

  long distance = duration * 0.034 / 2;
  return distance;
}