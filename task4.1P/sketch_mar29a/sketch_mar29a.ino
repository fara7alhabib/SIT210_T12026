#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

const int pirPin = 2;
const int switchPin = 3;
const int led1Pin = 5;
const int led2Pin = 6;

volatile bool pirTriggered = false;
volatile bool switchTriggered = false;

const float darkThreshold = 20.0;

unsigned long lastPirTime = 0;
unsigned long lastSwitchTime = 0;

void pirISR()
{
  unsigned long currentTime = millis();
  if (currentTime - lastPirTime > 2000)
  {
    pirTriggered = true;
    lastPirTime = currentTime;
  }
}

void switchISR()
{
  unsigned long currentTime = millis();
  if (currentTime - lastSwitchTime > 300)
  {
    switchTriggered = true;
    lastSwitchTime = currentTime;
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  Wire.begin();
  lightMeter.begin();

  pinMode(pirPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);

  attachInterrupt(digitalPinToInterrupt(pirPin), pirISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, FALLING);

  Serial.println("System ready...");
}

void loop()
{
  float lightLevel = lightMeter.readLightLevel();

  if (pirTriggered)
  {
    pirTriggered = false;

    if (lightLevel < darkThreshold)
    {
      digitalWrite(led1Pin, HIGH);
      digitalWrite(led2Pin, HIGH);

      Serial.print("Motion detected in DARK -> LEDs ON | Light: ");
      Serial.println(lightLevel);
    }
    else
    {
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);

      Serial.print("Motion detected but BRIGHT -> LEDs OFF | Light: ");
      Serial.println(lightLevel);
    }
  }

  if (switchTriggered)
  {
    switchTriggered = false;

    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);

    Serial.print("Switch used -> LEDs ON | Light: ");
    Serial.println(lightLevel);
  }
}