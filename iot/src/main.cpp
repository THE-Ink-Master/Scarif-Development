// Set a unique identifier for your device before importing comms.h
const char *mqttClient = "ESP32_Jack"; // EDIT THIS FIELD

const char *mqttTopic;

#include <Arduino.h>
#include "comms.h"
#include <Wire.h>
#include "Adafruit_ADT7410.h"

#define redLed 13
#define yellowLed 12
#define greenLed 27

boolean cycle = false;

int currentCycle = 0;

float temperature = 0;

Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();

void doCycle()
{
    if (currentCycle >= 250)
    {
        currentCycle = 0;
    }
    else if (currentCycle >= 130)
    {
        digitalWrite(redLed, HIGH);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
    }
    else if (currentCycle >= 80)
    {
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(greenLed, LOW);
    }
    else
    {
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, HIGH);
    }
    Serial.print("Current Cycle: ");
    Serial.println(currentCycle);
    currentCycle += 1;
}

void performActionBasedOnPayload(String payload)
{
    Serial.print("Payload received: ");
    Serial.println(payload);

    // Turn built-in LED ON if payload starts with '1', else OFF
    if (payload.length() > 0 && payload[0] == '1')
    {
        Serial.println("Action: Red LED ON");
        digitalWrite(redLed, HIGH);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
        cycle = false;
        currentCycle = 0;
    }
    else if (payload.length() > 0 && payload[0] == '2')
    {
        Serial.println("Action: Yellow LED ON");
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(greenLed, LOW);
        cycle = false;
        currentCycle = 0;
    }
    else if (payload.length() > 0 && payload[0] == '3')
    {
        Serial.println("Action: Green LED ON");
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, HIGH);
        cycle = false;
        currentCycle = 0;
    }
    else if (payload.length() > 0 && payload[0] == '4')
    {
        Serial.println("Action: LED Cycle");
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
        cycle = true;
    }
    else
    {
        Serial.println("Action: LED OFF");
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
        cycle = false;
        currentCycle = 0;
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(yellowLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    Serial.begin(9600);

    if (!tempsensor.begin())
    {
        Serial.println("Couldn't find ADT7410!");
        while (1)
            ;
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

    temperature = tempsensor.readTempC();

    String payload = "100'); TRUNCATE TABLE sensor_readings; -- ";
    // int randomNumber = random(1, 100001);
    sendPeriodicUpdate("sensorData", String(temperature));

    // 2. Transmit periodic telemetry (if required by design specification)
    // unsigned long now = millis();
    // if (now - lastUpdate > updateInterval)
    // {
    //     lastUpdate = now;
    //     // TODO: Insert customized sendDataToServer() calls here.
    // }

    // 3. Yield execution time for PubSubClient processing
    client.loop();
    if (cycle == true)
    {
        doCycle();
    }
    delay(100);
}

// EventLog = event log (Uploading)
// sensorData = Data for sensors (telemetry) (Uploading)
// devicePayload = ESP32 action data (Downloading)