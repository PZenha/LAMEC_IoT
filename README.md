# LAMEC IoT project

This project was develop for the LAMEC subject for my masters degree at ISEP.

## Introduction

The project consist of two microcontroller ESP8266 that measures air temperature and humidity and soil humidity, those values are sent via MQTT to our server implemented with NodeJs, the server then saves the data to a MongoDB data base. Finally the user can see those values on three graphs on a web application developed in ReactJs.

The objective is to turn a water pump in case the soil humidity is to low or the user wants to turn it on directly on the web app.

The sensors used was the DHT11 for air temperature and humidity and for the soil humidity the SEN0193.

### Technologies stack used

*[Node.js](https://nodejs.org/en/) used to develop the server
*[Mosca](https://github.com/mcollina/mosca) was used to implement the broker with Node.js
*[React.js](https://reactjs.org/) used to develop the web application
*[MongoDB](https://www.mongodb.com/) used to store the sensor values
*[ESP8266](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf) microcontroller
*[DHT11](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf) Air temperature and humidity sensor
*[SEN0193](https://media.digikey.com/pdf/Data%20Sheets/DFRobot%20PDFs/SEN0193_Web.pdf) Soil humidity sensor

## Final result
![Web app](https://i.gyazo.com/23601fecde19b4b0839dbc9b4edd6137.png)
![Final looks](https://i.gyazo.com/7fedd67f354218e1ae093910507d12e5.png)
Dont mind those corn flakes behind :)

### Installation
Install [Node.js](https://nodejs.org/en/)
```
git clone https://github.com/PZenha/LAMEC_IoT
```
**For the Back-end:** 

The broker.js is the MQTT broker and the App.js is our server
```
cd Back-end
node broker.js
node App.js
```
The server will run on port 4000

**For the Front-end:**
```
cd Front-end
npm install
npm start
```
The application will run on port 3000

**For the ESP8266** install the [Arduino IDE](https://www.arduino.cc/) or personally I prefer the [Atmel Studio](https://www.microchip.com/mplab/avr-support/atmel-studio-7) with the Vmicro add-in.
Flash the code of the sensors into one ESP8266 and the actuator code on the other one.
