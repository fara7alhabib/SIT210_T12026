const int livingRoomPin = 2;
const int bathroomPin = 3;
const int closetPin = 4;

void setup() {
  Serial.begin(9600);

  pinMode(livingRoomPin, OUTPUT);
  pinMode(bathroomPin, OUTPUT);
  pinMode(closetPin, OUTPUT);

  digitalWrite(livingRoomPin, LOW);
  digitalWrite(bathroomPin, LOW);
  digitalWrite(closetPin, LOW);
}

// FUNCTION REQUIRED BY TASK
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

void loop() {
  if (Serial.available()) {
    String room = Serial.readStringUntil('\n');
    room.trim();
    toggleLight(room);
  }
}