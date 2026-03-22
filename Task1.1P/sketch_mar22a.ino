const int led1 = 2;
const int led2 = 3;
const int button = 4;

void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button, INPUT_PULLUP);
}

void loop()
{
  if (digitalRead(button) == LOW)
  {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);

    delay(30000);   // 30 sec
    digitalWrite(led1, LOW);

    delay(30000);   // +30 = 60 sec
    digitalWrite(led2, LOW);
  }
}