/*
 Name:    ESP32_CANBUS_Capture.ino
 Created: 6/28/2021 10:53:47 AM
 Author:  Brandon Van Pelt
*/

// ESP32 CANBus Capture tool with serial controls
#include <esp32_can.h>

#define START 48 // 0
#define PAUSE 49 // 1
#define STOP  50 // 2

#define BAUD_500KBPS 500000

// CAN Bus Variables
CAN_FRAME message;
uint32_t messageNum = 0;
bool stopScan = true;
char string_out[200];
char temp1[10];
uint32_t temp = 0;

void readCAN()
{
    if (CAN0.read(message) && stopScan == false)
    {
        temp = millis();
        sprintf(string_out, "% -8d  % -9d  %04X   %d   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X",
            messageNum, temp, message.id, message.length, message.data.uint8[0], message.data.uint8[1],
            message.data.uint8[2], message.data.uint8[3], message.data.uint8[4], message.data.uint8[5],
            message.data.uint8[6], message.data.uint8[7]);
        Serial.println(string_out);
        messageNum++;
    }
}

void checkSerial()
{
    // User Serial Controls
    if (Serial.available())
    {
        uint8_t value = Serial.read();
        switch (value)
        {
        case START: stopScan = false;
            break;
        case PAUSE: stopScan = true;
            break;
        case STOP: stopScan = true;
                   messageNum = 0;
            break;
        }
    }
}

void setup()
{
    // Initialize Serial Monitor
    Serial.begin(BAUD_500KBPS);

    CAN0.begin(BAUD_500KBPS);
    CAN0.watchFor();
}

void loop()
{
    readCAN();
    checkSerial();
}