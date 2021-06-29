/*
 Name:		ESP32_CANBUS_Capture.ino
 Created:	6/28/2021 10:53:47 AM
 Author:	Brandon Van Pelt
*/

// ESP32 CANBus Capture tool with serial controls

// the setup function runs once when you press reset or power the board
#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

#define START 48 // 0
#define PAUSE 49 // 1
#define STOP  50 // 2

#define BAUD_500KBPS 500000
#define RX_BUFFER_SIZE 100

// CAN Bus Variables
CAN_device_t CAN0;         
CAN_frame_t rx_frame;
uint32_t messageNum = 0;
bool stopScan = false;

void setup()
{
    // Initialize Serial Monitor
    Serial.begin(BAUD_500KBPS);

    // CAN Bus setup
    CAN0.speed = CAN_SPEED_500KBPS;
    CAN0.tx_pin_id = GPIO_NUM_25;
    CAN0.rx_pin_id = GPIO_NUM_26;
    CAN0.rx_queue = xQueueCreate(RX_BUFFER_SIZE, sizeof(CAN_frame_t));

    // Init CAN Module
    ESP32Can.CANInit();
}

void loop()
{
    // Receive next CAN frame from queue
    if (xQueueReceive(CAN0.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE && stopScan == false)
    {
        char string_out[200];
        char temp1[10];
        uint32_t temp = millis();

        sprintf(string_out, "% -8d  % -9d  %04X   %d   %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X",
            messageNum, temp, rx_frame.MsgID, rx_frame.FIR.B.DLC, rx_frame.data.u8[0],
            rx_frame.data.u8[1], rx_frame.data.u8[2], rx_frame.data.u8[3], rx_frame.data.u8[4],
            rx_frame.data.u8[5], rx_frame.data.u8[6], rx_frame.data.u8[7]);
        Serial.println(string_out);

        messageNum++;
    }

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