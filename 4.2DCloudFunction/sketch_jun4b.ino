#include <WiFiNINA.h>
#include <PubSubClient.h>

char ssid[] = "Joyiphone";
char pass[] = "123456778";

const int livingRoomPin = 2;
const int bathroomPin = 3;
const int closetPin = 4;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topic = "farah/linda/lights";

void setup() {
  Serial.begin(9600);

  pinMode(livingRoomPin, OUTPUT);
  pinMode(bathroomPin, OUTPUT);
  pinMode(closetPin, OUTPUT);

  digitalWrite(livingRoomPin, LOW);
  digitalWrite(bathroomPin, LOW);
  digitalWrite(closetPin, LOW);

  connectWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }

  Serial.println();
  Serial.println("Connected to WiFi");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    if (client.connect("LindaLightsArduino")) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String room = "";

  for (int i = 0; i < length; i++) {
    room += (char)message[i];
  }

  Serial.print("Message received: ");
  Serial.println(room);

  toggleLight(room);
}

void toggleLight(String room) {
  if (room == "living room") {
    digitalWrite(livingRoomPin, !digitalRead(livingRoomPin));
  }
  else if (room == "bathroom") {
    digitalWrite(bathroomPin, !digitalRead(bathroomPin));
  }
  else if (room == "closet") {
    digitalWrite(closetPin, !digitalRead(closetPin));
  }
}