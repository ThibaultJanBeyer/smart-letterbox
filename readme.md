# Arduino Smart Letterbox / Mailbox Tutorial

Based on this video: https://www.youtube.com/watch?v=fdkyScnZ_w4

## Required Hardware

- DFRobot FireBeetle ESP32 IOT MicroController. Or the cheaper ESP8266
- 2x Snap-Action Switch – NC Switch (any kind will do)
- Wires

## Steps

### Install Pub/Sub Library

- Open Arduino IDE
- Go to Tools > Manage Libraries
- Search for `PubSubClient` and get the one by Nick O'Leary
- Install

### Link FireBeetle with Arduino

#### For the ESP32:

https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

- Open Arduino IDE
- Go to Settings/Preferences
- Under “Additional board URLs” add both: https://git.oschina.net/dfrobot/FireBeetle-ESP32/raw/master/package_esp32_index.json,https://dl.espressif.com/dl/package_esp32_index.json
- Go to Tools > Boards > Boards Manager
- Search for FireBeetle (or esp32), install it
- Now Tools > Board > Select the FireBeetle Board

### Connect your ESP32 with the switches:


