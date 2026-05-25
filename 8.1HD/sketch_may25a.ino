#include <ArduinoBLE.h>
#include <BH1750.h>
#include <Wire.h>

// =======================
// LIGHT SENSOR
// =======================
BH1750 lightMeter;

// =======================
// LED PINS
// =======================
int bathroomLED = 2;
int hallwayLED = 3;
int roomLED = 4;
int fanLED = 5;

// =======================
// DARKNESS LIMIT
// =======================
float darkLimit = 20;

// =======================
// BLE SERVICE
// =======================
BLEService lightService("19B10000-E8F2-537E-4F6C-D104768A1214");

// =======================
// BLE CHARACTERISTIC
// =======================
BLEStringCharacteristic commandCharacteristic(
  "19B10001-E8F2-537E-4F6C-D104768A1214",
  BLERead | BLEWrite,
  50
);

void setup()
{
  Serial.begin(9600);

  // =======================
  // LED OUTPUTS
  // =======================
  pinMode(bathroomLED, OUTPUT);
  pinMode(hallwayLED, OUTPUT);
  pinMode(roomLED, OUTPUT);
  pinMode(fanLED, OUTPUT);

  // =======================
  // SENSOR START
  // =======================
  Wire.begin();

  if (lightMeter.begin())
  {
    Serial.println("BH1750 ready");
  }
  else
  {
    Serial.println("[BH1750] Device is not configured!");
  }

  // =======================
  // START BLE
  // =======================
  if (!BLE.begin())
  {
    Serial.println("Bluetooth failed to start");
    while (1);
  }

  BLE.setLocalName("VoiceLightSystem");

  BLE.setAdvertisedService(lightService);

  lightService.addCharacteristic(commandCharacteristic);

  BLE.addService(lightService);

  commandCharacteristic.writeValue("none");

  BLE.advertise();

  Serial.println("Arduino BLE is ready");
  Serial.println("Waiting for Raspberry Pi command...");
}

void loop()
{
  BLEDevice central = BLE.central();

  if (central)
  {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected())
    {
      // =======================
      // CHECK FOR COMMAND
      // =======================
      if (commandCharacteristic.written())
      {
        String command = commandCharacteristic.value();

        // =======================
        // SENSOR VALUE
        // =======================
        float lux = lightMeter.readLightLevel();

        Serial.print("Command received: ");
        Serial.println(command);

        Serial.print("Light sensor value: ");
        Serial.println(lux);

        // =======================
        // BATHROOM ON
        // =======================
        if (command == "bathroom_on")
        {
          if (lux < darkLimit)
          {
            digitalWrite(bathroomLED, HIGH);
            Serial.println("Bathroom light ON");
          }
          else
          {
            Serial.println("Too bright for bathroom light");
          }
        }

        // =======================
        // BATHROOM OFF
        // =======================
        else if (command == "bathroom_off")
        {
          digitalWrite(bathroomLED, LOW);
          Serial.println("Bathroom light OFF");
        }

        // =======================
        // HALLWAY ON
        // =======================
        else if (command == "hallway_on")
        {
          if (lux < darkLimit)
          {
            digitalWrite(hallwayLED, HIGH);
            Serial.println("Hallway light ON");
          }
          else
          {
            Serial.println("Too bright for hallway light");
          }
        }

        // =======================
        // HALLWAY OFF
        // =======================
        else if (command == "hallway_off")
        {
          digitalWrite(hallwayLED, LOW);
          Serial.println("Hallway light OFF");
        }

        // =======================
        // ROOM ON
        // =======================
        else if (command == "room_on")
        {
          if (lux < darkLimit)
          {
            digitalWrite(roomLED, HIGH);
            Serial.println("Room light ON");
          }
          else
          {
            Serial.println("Too bright for room light");
          }
        }

        // =======================
        // ROOM OFF
        // =======================
        else if (command == "room_off")
        {
          digitalWrite(roomLED, LOW);
          Serial.println("Room light OFF");
        }

        // =======================
        // FAN ON
        // =======================
        else if (command == "fan_on")
        {
          digitalWrite(fanLED, HIGH);
          Serial.println("Fan ON");
        }

        // =======================
        // FAN OFF
        // =======================
        else if (command == "fan_off")
        {
          digitalWrite(fanLED, LOW);
          Serial.println("Fan OFF");
        }

        // =======================
        // ALL ON
        // =======================
        else if (command == "all_on")
        {
          digitalWrite(bathroomLED, HIGH);
          digitalWrite(hallwayLED, HIGH);
          digitalWrite(roomLED, HIGH);
          digitalWrite(fanLED, HIGH);

          Serial.println("Everything ON");
        }

        // =======================
        // ALL OFF
        // =======================
        else if (command == "all_off")
        {
          digitalWrite(bathroomLED, LOW);
          digitalWrite(hallwayLED, LOW);
          digitalWrite(roomLED, LOW);
          digitalWrite(fanLED, LOW);

          Serial.println("Everything OFF");
        }
      }
    }

    Serial.println("Disconnected");
  }
}