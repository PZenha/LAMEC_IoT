

/*
    Name:       LAMEC_ESP8266.ino
    Created:	06/11/2019 15:31:39
    Author:     Pedro Zenha, André Makrilou
	Subject: LAMEC
	Project: IoT platform
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_Sensor.h> //Dependence for dht library
#include <string.h>


#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);

const char* ssid = "pedro";
const char* password = "123456789";
const char* mqttServer ="192.168.43.135";   //"farmer.cloudmqtt.com";
const int mqttPort = 1883;
//const char* mqttUser = "pwimkkdw";
//const char* mqttPassword = "nNDIkzJDsmVp"; 

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);
void TempAndHumi();
void SoilMoisture();

void TempAndHumi(){
	//String dataS ="";
	char* data ="";
	float h = dht.readHumidity();	//Reads humidity value
	float t = dht.readTemperature();	//Reads Temp. values
	
	if(isnan(h) || isnan(t)){
		Serial.println("Failed to read DHT11 data!");
		return;
	}
	
	Serial.print("Temperature: ");
	Serial.print(t);
	Serial.println(" C");
	Serial.print("Humidity: ");
	Serial.print(h);
	Serial.println("%");
	sprintf(data,"T_air:%.2f H_air:%.2f",t,h);
	Serial.println("");
	client.publish("esp/test", data);
}

void SoilMoisture(){
	const float AirValue = 702; //Value measured on air, DRY
	const float WaterValue = 521; //Value measured on water, WET
	float soilPercentage = 0;
	char* data ="";
	int moistureValue = analogRead(A0);  // [0;1023]
	float Interval = AirValue - WaterValue;   //Value measured minus lowest number of the range interval
	soilPercentage = ((AirValue - (float)moistureValue)/Interval) * 100.0;   //Transform value measured into percentage
	Serial.print("Soil Moisture: ");
	Serial.print(moistureValue);
	Serial.print(" - ");
	Serial.print(soilPercentage);
	Serial.println("%");
	sprintf(data,"soil:%.2f",soilPercentage);
	client.publish("esp/test", data);
	Serial.println("");
}

void setup(){
	Serial.begin(115200);
	dht.begin(); 
	delay(3000);
	/*WiFi.begin(ssid, password);
	Serial.print("Connecting to ");
	Serial.print(ssid);
	int attempt = 0;
	while(WiFi.status() != WL_CONNECTED && attempt<100){
		delay(100);
		Serial.print(".");
		attempt++;
	}
	if(WiFi.status() == WL_CONNECTED){
		Serial.println("");
		Serial.println("WiFi Connected!");
		Serial.print("Local IP: ");
		Serial.println(WiFi.localIP());
	}
	if(attempt == 100){
		Serial.println("Failed to connect to WiFi");
	}
	
	client.setServer(mqttServer,mqttPort);
	client.setCallback(callback);
	while(!client.connected()){
		Serial.println("Connecting to MQTT...");
		delay(200);
		if(client.connect("ESP8266Client")){
			Serial.println("Connected to MQTT");
		}else{
			Serial.println("Failed to connect to mqtt");
			Serial.println(client.state());
		}
	}
	Serial.println("Connected to MQTT broker");
	client.publish("esp/test", "hello");
	client.subscribe("esp/test"); */
	
}



// Add the main program code into the continuous loop() function
void loop(){
	client.loop();
	TempAndHumi(); //Returns temp and humidity values
	SoilMoisture();
	delay(2000);
	
	
}


void callback(char* topic, byte* payload, unsigned int length) {
	
	Serial.print("Message arrived in topic: ");
	Serial.println(topic);
	
	Serial.print("Message:");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	
	Serial.println();
	Serial.println("-----------------------");
	
}