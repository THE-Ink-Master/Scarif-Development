#include <Arduino.h>
#include <PubSubClient.h>

#define pinLed 13

void led()
{
  digitalWrite(pinLed, HIGH);
  delay(500);
  digitalWrite(pinLed, LOW);
  delay(500);
}

void setup()
{
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  led();
}