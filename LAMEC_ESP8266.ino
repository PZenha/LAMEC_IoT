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
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);
ESP8266WebServer server(80);
HTTPClient http;
const char HTML_PAGE[] PROGMEM = "<!DOCTYPE html>\n<html>\n<h1 align=\"center\">Controlo de Temperatura e Humidade</h1>\n<br>\n<h3>Dados atuais</h3>\n<p>Temperatura do ar: {T_air} Cº</p>\n<p>Humidade do ar: {H_air}%</p>\n<p>Humidade do solo: {H_soil}%</p>\n<br>\n<b>Ligar motor</b>\n<form action=\"/\" method=\"GET\">\n  <button name=\"t\" value=\"on\" type=\"submit\">ON</button>\n  <br>\n  <button name=\"t\" value=\"off\" type=\"submit\">OFF</button>\n</form>\n</html>";

const char* ssid = "Zenha HOTSPOT";//"pedro";//"MEO-CF0925";	
const char* password ="123zenha456";//"123456789";//"9F9764AC4B";
const char* nodeSSID = "ESPNode";
const char* mqttServer ="192.168.8.100";//"192.168.43.135";	//"192.168.1.119";	//"192.168.1.74";
const int mqttPort = 1883;
boolean flag_NoWiFi = true;
boolean flag_NoServer = true;
boolean flag_OfflineTimer = false;
boolean flag_lowWater = false;
boolean flag_FoundSSID = false;
boolean flag_FoundSSIDWiFi = false;
boolean flag_connectedToNode = false;
boolean flag_WebServerON = false;

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t time_start = 0;
uint32_t OfflineStart = 0;
boolean TimeIsRunning = false;

String globalHumi="";
String globalTemp="";
String globalSoil="";

unsigned short int timerCounter = 0;
unsigned short int timerMesh = 0;

void checkForWiFiConnection();
void callback(char* topic, byte* payload, unsigned int length);
void handleRoot();
void sendDataToNode(String serverHost);
void connectToWiFi();
void connectToMeshNode();
void WebServerStart();

String TempAndHumi(){
	float h = dht.readHumidity();	//Reads humidity value
	float t = dht.readTemperature();	//Reads Temp. values
	globalHumi = String(h);
	globalTemp = String(t);
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
	globalSoil = String(soilPercentage);
	Serial.print("Soil Moisture: ");
	Serial.print(moistureValue);
	Serial.print(" - ");
	Serial.print(soilPercentage);
	Serial.println("%");
	String dataS = " soil:";
	dataS += soilPercentage;
	
	if(soilPercentage < 10){
		flag_lowWater = true;
	}
	
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
	WiFi.mode(WIFI_AP_STA);
	WiFi.begin(ssid, password);
	WiFi.setAutoConnect(true);
	Serial.print("Connecting to ");
	Serial.print(ssid);
	int attempt = 0;
	while(WiFi.status() != WL_CONNECTED && attempt<150){  //Connecting to Wi-Fi
		delay(100);
		Serial.print(".");
		attempt++;
	}
	if(WiFi.status() == WL_CONNECTED){
		Serial.println("");
		Serial.println("WiFi Connected!");
		Serial.print("Local IP: ");
		Serial.println(WiFi.localIP());
		flag_NoWiFi = false;
	}
	if(attempt == 150){
		Serial.println("Failed to connect to WiFi...Starting WebServer...");
		flag_NoWiFi = true;
		TempAndHumi();
		SoilMoisture();
	}
	
	if(!flag_NoWiFi){
		unsigned short int mqtt_attempt = 0;
		client.setServer(mqttServer,mqttPort);
		client.setCallback(callback);
		while(!client.connected() && mqtt_attempt != 10){  //Connecting to MQTT
			Serial.println("Connecting to MQTT...");
			delay(200);
			if(client.connect("ESP8266Client")){
				Serial.println("Connected to MQTT");
				client.publish("esp/sensor", "Hello I am alive");
				client.subscribe("esp/sensor");
				flag_NoServer = false;
			}else{
				Serial.println("Failed to connect to mqtt");
				Serial.println(client.state());
				mqtt_attempt++;
			}
		}
	}
	
}



// Add the main program code into the continuous loop() function
void loop(){
		
	if(!flag_NoWiFi){
		delay(1);
		client.loop();
		checkForWiFiConnection();
		if(!TimeIsRunning){
			time_start = millis();
			TimeIsRunning = true;
		}
		
		if(millis() - time_start >= 5000 && TimeIsRunning == true){   //If time passed is greater than 5 seconds
			TimeIsRunning = false;
			SendDataByMqtt(TempAndHumi(),SoilMoisture());
		}
	}
	
	if(flag_NoWiFi){
		delay(1);
		
		if(!flag_WebServerON){
			WebServerStart(); //Starts webserver and mesh network if internet goes down
		}
		server.handleClient();
		if(!flag_connectedToNode){
			connectToMeshNode();
		}
		if(!TimeIsRunning){
			OfflineStart = millis();
			TimeIsRunning = true;
		}
		if(millis() - OfflineStart >= 1000 && TimeIsRunning == true){
			yield();
			timerCounter++;
			timerMesh++;
			Serial.print("TimerCounter:");
			Serial.println(timerCounter);
			Serial.println(ESP.getFreeHeap());
			TimeIsRunning = false;
			TempAndHumi();
			SoilMoisture();
			if(globalSoil.toInt() < 5 && flag_connectedToNode && timerCounter == 20){
				sendDataToNode("http://192.168.10.10/water");
			}
		}
		if(timerCounter == 30){
			connectToWiFi();
			timerCounter = 0;
		}
	}
	
}

void checkForWiFiConnection(){
	if(WiFi.status() != WL_CONNECTED){
		flag_NoWiFi = true;
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

void handleRoot(){
	String page = FPSTR(HTML_PAGE);
	page.replace("{T_air}",globalTemp);
	page.replace("{H_air}",globalHumi);
	page.replace("{H_soil}",globalSoil);
	server.sendHeader("Content-Length", String(page.length()));
	server.send(200,"text/html", page);
	String action = server.arg("t").c_str();
	
	if(action == "on"){
		sendDataToNode("http://192.168.10.10/water");
	}
}

void sendDataToNode(String serverHost){
		Serial.println("Sending Data to Node...");
		http.begin(serverHost);
		http.addHeader("Content-Type", "application/x-www-form-urlencoded");
		int httpCode = http.GET();
		if(httpCode > 0){
			String payload = http.getString();
			Serial.println(payload);
		}
		http.end();
}

void connectToWiFi(){
	int n = WiFi.scanNetworks(); //n equals number of networks found
	if(n != 0){
		for(int i=0; i < n; i++){
			Serial.println(WiFi.SSID(i));
			if(WiFi.SSID(i) == "Zenha HOTSPOT"){
				flag_FoundSSIDWiFi = true;
			}
		}
	}
	if(flag_FoundSSIDWiFi){
		WiFi.disconnect();
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
			flag_NoWiFi = false;
			flag_WebServerON = false;
			flag_FoundSSIDWiFi = false;
			server.stop(); //Shutdown webserver
		}else{
			flag_connectedToNode = false;
		}
		if(!flag_NoWiFi){
			unsigned short int mqtt_attempt = 0;
			client.setServer(mqttServer,mqttPort);
			client.setCallback(callback);
			while(!client.connected() && mqtt_attempt != 10){  //Connecting to MQTT
				Serial.println("Connecting to MQTT...");
				delay(200);
				if(client.connect("ESP8266Client")){
					Serial.println("Connected to MQTT");
					client.publish("esp/sensor", "Hello I am alive");
					client.subscribe("esp/sensor");
					flag_NoServer = false;
					}else{
						Serial.println("Failed to connect to mqtt");
						Serial.println(client.state());
						mqtt_attempt++;
					}
			}
		}
	}
}

void connectToMeshNode(){
	if(timerMesh == 10){
		timerMesh = 0;
		int n = WiFi.scanNetworks(); //n equals number of networks found
		if(n != 0){
			for(int i=0; i < n; i++){
				Serial.println(WiFi.SSID(i));
				if(WiFi.SSID(i) == "ESPNode"){
					flag_FoundSSID = true;
				}
			}
		}
			if(flag_FoundSSID){
				int attempt = 0;
				WiFi.begin(nodeSSID);
				Serial.print("Connecting to ESPNode");
				while(WiFi.status() != WL_CONNECTED && attempt != 200){
					Serial.print("*");
					delay(100);
					attempt++;
				}
				if(attempt == 200){
					Serial.println("Failed to connect to ESPNode...");
					Serial.print("Fail status:");
					Serial.println(WiFi.status());
				}else{
					Serial.println("Connected to ESPNode!");
					flag_connectedToNode = true;
				}
			}
		}
	}

void WebServerStart(){
	Serial.println("Starting WebServer...");
	WiFi.softAP("ESP8266 Sensores");
	Serial.println(WiFi.softAPIP());
	server.on("/",handleRoot);
	server.begin();
	flag_WebServerON = true;
}

