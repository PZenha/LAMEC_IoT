

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

const char* ssid = "pedro";		//"MEO-CF0925";	
const char* password ="123456789";	 //"9F9764AC4B";
const char* mqttServer ="172.20.10.3";//"192.168.43.135";	//"192.168.1.119";	
const int mqttPort = 1883;
//const char* mqttUser = "pwimkkdw";
//const char* mqttPassword = "nNDIkzJDsmVp"; 

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t time_start = 0;
boolean TimeIsRunning = false;

void callback(char* topic, byte* payload, unsigned int length);
//void TempAndHumi();
//void SoilMoisture();

String TempAndHumi(){
	float h = dht.readHumidity();	//Reads humidity value
	float t = dht.readTemperature();	//Reads Temp. values
	
	if(isnan(h) || isnan(t)){
		Serial.println("Failed to read DHT11 data!");
		return "";
	}
	String dataS = "T_air:";
	dataS += t;
	dataS += " H_air:";
	dataS += h;
	Serial.print("Temperature: ");
	Serial.print(t);
	Serial.println(" C");
	Serial.print("Humidity: ");
	Serial.print(h);
	Serial.println("%");
	return dataS;
}

String SoilMoisture(){
	const float AirValue = 712; //Value measured on air, DRY
	const float WaterValue = 520; //Value measured on water, WET
	short int soilPercentage = 0;
	int moistureValue = analogRead(A0);  // [0;1023]
	float Interval = AirValue - WaterValue;   //Value measured minus lowest number of the range interval
	soilPercentage = ((AirValue - (float)moistureValue)/Interval) * 100.0;   //Transform value measured into percentage
	Serial.print("Soil Moisture: ");
	Serial.print(moistureValue);
	Serial.print(" - ");
	Serial.print(soilPercentage);
	Serial.println("%");
	String dataS = " soil:";
	dataS += soilPercentage;
	return dataS;
}

void SendDataByMqtt(String TH_air, String soilS){
	String dataS = TH_air + soilS;
	const char* data = dataS.c_str();
	client.publish("esp/sensor", data);
}


void setup(){
	Serial.begin(115200);
	dht.begin(); 
	delay(3000);
	WiFi.begin(ssid, password);
	Serial.print("Connecting to ");
	Serial.print(ssid);
	int attempt = 0;
	while(WiFi.status() != WL_CONNECTED && attempt<100){  //Connecting to Wi-Fi
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
	while(!client.connected()){  //Connecting to MQTT
		Serial.println("Connecting to MQTT...");
		delay(200);
		if(client.connect("ESP8266Client")){
			Serial.println("Connected to MQTT");
		}else{
			Serial.println("Failed to connect to mqtt");
			Serial.println(client.state());
		}
	}
	client.publish("esp/sensor", "Hello I am alive");
	client.subscribe("esp/sensor"); 
	
}



// Add the main program code into the continuous loop() function
void loop(){
	client.loop();
	
	if(!TimeIsRunning){
		 time_start = millis();
		 TimeIsRunning = true;
	}
	
	if(millis() - time_start >= 5000 && TimeIsRunning == true){   //If time passed is greater than 1 minute 
		TimeIsRunning = false;
		SendDataByMqtt(TempAndHumi(),SoilMoisture());
	}
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