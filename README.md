# LAMEC_IoT

This project was develop for the LAMEC subject for my masters degree at ISEP.

## Introduction

The project consist of two microcontroller ESP8266 that measures air temperature and humidity and soil humidity, those values are sent via MQTT to our server implemented with NodeJs, the server then saves the data to a MongoDB data base. Finally the user can see those values on three graphs on a web application developed in ReactJs.

The objective is to turn a water pump in case the soil humidity is to low or the user wants to turn it on directly on the web app.

## Final result
![Web app](https://i.gyazo.com/23601fecde19b4b0839dbc9b4edd6137.png)
![Final looks](https://i.gyazo.com/7fedd67f354218e1ae093910507d12e5.png)
Dont mind those corn flakes behind :)

### Installation
Install [Node.js](https://nodejs.org/en/)
```
git clone https://github.com/PZenha/LAMEC_IoT
```
To run the back-end:
The broker.js is the MQTT broker and the App.js is our server
```
cd Back-end
node broker.js
node App.js
```
The server will run on port 4000

For the Front-end:
```
cd Front-end
npm install
npm start
```
The application will run on port 3000

For the ESP8266 install the [Arduino IDE](https://www.arduino.cc/) or personally I prefer the [Atmel Studio](https://www.microchip.com/mplab/avr-support/atmel-studio-7) with the Vmicro add-in.
Flash the code of the sensors into one ESP8266 and the actuator code on the other one.
