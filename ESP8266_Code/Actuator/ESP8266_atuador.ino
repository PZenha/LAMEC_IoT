// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       ESP8266_atuador.ino
    Created:	03/12/2019 16:41:33
    Author:     Pedro Zenha, André Makrilou
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "Zenha HOTSPOT";	//"MEO-CF0925"; //;//	//
const char* password = "123zenha456";//"9F9764AC4B";	//"123456789"; 
const char* mqttServer ="192.168.8.100"; //"192.168.1.119"; // ;//;//	"192.168.43.135";   //"farmer.cloudmqtt.com";
const int mqttPort = 1883;

IPAddress apIP(192,168,10,10);

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);
HTTPClient http;

String mqttRequest = "";
boolean TimeIsRunning = false;
boolean flag_on = false;
boolean flag_NoWiFi = true;
boolean flag_NoServer = true;
boolean flag_AP = false;
boolean flag_FoundSSID = false;
unsigned long int TimeRunning = 0;
unsigned long int OfflineStart = 0;
short int timerCounter = 0;

void checkForWiFiConnection();
void connectToWiFi();
void callback(char* topic, byte* payload, unsigned int length);
void actuator();
void TurnRelayOFF();
void handleWater();
void startAccessPoint();

void setup(){
	pinMode(4, OUTPUT);
	digitalWrite(4,HIGH);
	Serial.begin(115200);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0)); //Set softAP 192.168.10.10
	WiFi.begin(ssid, password);
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
		flag_NoWiFi = false;
	}
	if(attempt == 100){
		Serial.println("Failed to connect to WiFi");
	}
	
	if(!flag_NoWiFi){
		unsigned short int mqtt_attempt = 0;
		client.setServer(mqttServer,mqttPort);
		client.setCallback(callback);
		while(!client.connected()){
			Serial.println("Connecting to MQTT...");
			delay(200);
			if(client.connect("ESP8266Client")){
				flag_NoServer = false;
				Serial.println("Connected to MQTT broker");
				client.publish("esp/actuator", "Hello from the ESP Actuator");
				client.subscribe("esp/actuator");
				}else{
					Serial.println("Failed to connect to mqtt");
					Serial.println(client.state());
					mqtt_attempt++;
					
			}
		}
	}
}

// Add the main program code into the continuous loop() function
void loop()
{
	yield();
	if(!flag_NoWiFi){
		int mqtt_attempt = 0;
		client.loop();
		TurnRelayOFF();
		checkForWiFiConnection();
		if(client.state() != 0){
			while(!client.connected() && mqtt_attempt != 10){  //Connecting to MQTT
				Serial.println("Connecting to MQTT...");
				delay(200);
				if(client.connect("ESP8266Client")){
					Serial.println("Connected to MQTT");
					client.subscribe("esp/actuator");
					flag_NoServer = false;
					}else{
					Serial.println("Failed to connect to mqtt");
					Serial.println(client.state());
					mqtt_attempt++;
				}
			}
		}
	}
	
	if(flag_NoWiFi){
		delay(1);
		yield();
		if(!flag_AP){
			startAccessPoint();
		}
		server.handleClient();
		TurnRelayOFF();
		
		if(!TimeIsRunning){
			OfflineStart = millis();
			TimeIsRunning = true;
		}
		
		if(millis() - OfflineStart >= 1000 && TimeIsRunning){
			timerCounter++;
			Serial.print("timerCounter:");
			Serial.println(timerCounter);
			TimeIsRunning = false;
		}
		
		if(timerCounter == 180){
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

void connectToWiFi(){
	int n = WiFi.scanNetworks(); //n equals number of networks found
	if(n != 0){
		for(int i=0; i < n; i++){
			Serial.println(WiFi.SSID(i));
			if(WiFi.SSID(i) == "Zenha HOTSPOT"){
				flag_FoundSSID = true;
			}
		}
	}
	if(flag_FoundSSID){
		int attempt = 0;
		WiFi.begin(ssid,password);
		Serial.print("Connecting to Zenha HOTSPOT");
		while(WiFi.status() != WL_CONNECTED && attempt != 150){
			Serial.print("*");
			delay(100);
			attempt++;
		}
		if(attempt == 150){
			Serial.println("Failed to connect to WiFi...");
			Serial.print("Fail status:");
			Serial.println(WiFi.status());
		}else{
			Serial.println("Connected to WiFi!");
			flag_NoWiFi = false;
		}
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
				client.publish("esp/sensor", "Hello from ESP");
				client.subscribe("esp/actuator");
				flag_NoServer = false;
				}else{
				Serial.println("Failed to connect to mqtt");
				Serial.println(client.state());
				mqtt_attempt++;
			}
		}
	}
}

void callback(char* topic, byte* payload, unsigned int length) {
	
	
	Serial.print("Message arrived in topic: ");
	Serial.println(topic);
	
	Serial.print("Message:");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
		mqttRequest += (char)payload[i];
	}
	actuator();
	Serial.println();
	Serial.println("-----------------------");
	
}

void actuator(){
	
	if(mqttRequest.equals("ON")){
		Serial.println("Relay ON");
		digitalWrite(4,LOW); //Abre
		mqttRequest = "";
		TimeRunning = millis();
		flag_on = true;
	}
	if(mqttRequest.equals("OFF")){
		Serial.println("Relay OFF");
		digitalWrite(4,HIGH);
		mqttRequest="";
		flag_on = false;
	}
}

void TurnRelayOFF(){
	if(flag_on == true && TimeRunning > 0){
		if(millis() - TimeRunning >= 2500){
			digitalWrite(4,HIGH);
			Serial.println("Relay OFF");
			flag_on = false;
			TimeRunning = 0;
		}
	}
	
}
void handleRoot(){
	server.send(200,"text/plain","You're on root, go to /water or /turnoffwater");
}

void handleWater(){
	server.send(200,"text/plain", "OK");
	Serial.println("Relay ON");
	digitalWrite(4,LOW); //Abre
	TimeRunning = millis();
	flag_on = true;
}

void startAccessPoint(){
	Serial.println("Starting AccessPoint...");
	WiFi.softAP("ESPNode");
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("softAP IP:");
	Serial.println(myIP);
	server.on("/",handleRoot);
	server.on("/water",handleWater);
	server.begin();
	flag_AP = true;
}