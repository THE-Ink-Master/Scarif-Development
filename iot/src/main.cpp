// Set a unique identifier for your device before importing comms.h
const char *mqttClient = "ESP32_Jack"; // EDIT THIS FIELD

const char *mqttTopic;

#include <Arduino.h>
#include "comms.h"
#include <Wire.h>
#include "Adafruit_ADT7410.h"

float temp = 0;

Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();

void performActionBasedOnPayload(String payload)
{
    Serial.print("Payload received: ");
    Serial.println(payload);

    // Turn built-in LED ON if payload starts with '1', else OFF
    if (payload.length() > 0 && payload[0] == '1')
    {
        Serial.println("Action: LED ON");
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        Serial.println("Action: LED OFF");
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);

    if (!tempsensor.begin())
    {
        Serial.println("Couldn't find ADT7410!");
        while (1);
    }
    delay(250);

    wifiSetup();
    mqttSetup();

    while (!Serial)
    {
        delay(10);
    }
    delay(1000);

    // randomSeed(analogRead(A0));
}

void loop()
{
    // 1. Maintain connection to the broker
    mqttConnect();

    temp = tempsensor.readTempC();
    // int randomNumber = random(1, 100001);
    sendPeriodicUpdate("sensorData", String(temp));

    // 2. Transmit periodic telemetry (if required by design specification)
    // unsigned long now = millis();
    // if (now - lastUpdate > updateInterval)
    // {
    //     lastUpdate = now;
        // TODO: Insert customized sendDataToServer() calls here.
    // }

    // 3. Yield execution time for PubSubClient processing
    client.loop();
    delay(100);
}

// EventLog = event log (Uploading)
// sensorData = Data for sensors (telemetry) (Uploading)
// devicePayload = ESP32 action data (Downloading)