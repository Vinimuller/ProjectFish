#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <string.h>


struct s_fishStatus fishStatus;
struct s_fishConfig fishConfig;

#define RELAY_PIN	15
#define LED_R_PIN	15
#define LED_G_PIN	12 
#define LED_B_PIN	13

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "raks" // Put you SSID and Password here
	#define WIFI_PWD "13092017"
#endif

// ... and/or MQTT username and password
#ifndef MQTT_USERNAME
	#define MQTT_USERNAME ""
	#define MQTT_PWD ""
#endif

#ifndef MQTT_TOPIC_SUB_DATA
	#define MQTT_TOPIC_SUB_DATA "U7886zhUcV_fish_house/data"
#endif

#ifndef MQTT_TOPIC_SUB_CONFIG
	#define MQTT_TOPIC_SUB_CONFIG "U7886zhUcV_fish_house/config"
#endif

#ifndef MQTT_TOPIC_PUBLISH
	#define MQTT_TOPIC_PUBLISH "U7886zhUcV_fish_house/data"
#endif

#ifndef MQTT_ID
	#define MQTT_ID "ESP_vini"
#endif

// ... and/or MQTT host and port
#ifndef MQTT_HOST
	#define MQTT_HOST "iot.eclipse.org"
	#define MQTT_PORT 1883
#endif

// Forward declarations
void startMqttClient();
void wifiConnect();
void onMessageReceived(String topic, String message);

Timer loopTemperatureControlTimer;
Timer procTimer;
Timer reconnectTimer;

// MQTT client
// For quick check you can use: http://www.hivemq.com/demos/websocket-client/ (Connection= test.mosquitto.org:8080)
MqttClient *mqtt;

//Get value from ADC and transform into Celsius
float getTemperature()
{
	int adc = 0;

	adc = analogRead(A0);	

	return fishConfig.temperature_a_coeficient*adc + fishConfig.temperature_b_coeficient;
}

// Check for MQTT Disconnection
void checkMQTTDisconnect(TcpClient& client, bool flag){
	
	// Called whenever MQTT connection is failed.
	if (flag == true)
		Serial.println("MQTT Broker Disconnected!!\n\n");
	else
		Serial.println("MQTT Broker Unreachable!!\n\n");
	
	// Restart connection attempt after few seconds
	procTimer.initializeMs(2 * 1000, startMqttClient).start(); // every 2 seconds
}

void onMessageDelivered(uint16_t msgId, int type) {
	Serial.printf("Message with id %d and QoS %d was delivered successfully.\n\n", msgId, (type==MQTT_MSG_PUBREC? 2: 1));
}

void updateFishData()
{
	fishStatus.temperature 			= getTemperature();
	fishStatus.relayStatus			= digitalRead(RELAY_PIN); 
}

// Publish our message
void publishMessage()
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	
	updateFishData();

	root["setPointTemperature"]		= fishStatus.setPointTemperature;
	root["deadBandTemperature"]		= fishStatus.deadBandTemperature;
	root["temperature"]				= fishStatus.temperature;
	root["autoTemperatureControl"]	= fishStatus.autoTemperatureControl;
	root["relayStatus"]				= fishStatus.relayStatus;

	String tempStr;
	root.printTo(tempStr);

	if (mqtt->getConnectionState() != eTCS_Connected)
		startMqttClient(); // Auto reconnect

	mqtt->publishWithQoS(MQTT_TOPIC_PUBLISH,tempStr,1,false,onMessageDelivered); 
}

// Callback for messages, arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	if(topic == MQTT_TOPIC_SUB_DATA)
	{
		String tempStr;
		StaticJsonBuffer<500> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(message);

		fishStatus.setPointTemperature		= root["setPointTemperature"];
		fishStatus.deadBandTemperature 		= root["deadBandTemperature"];		
		fishStatus.temperature 				= root["temperature"];
		fishStatus.autoTemperatureControl 	= root["autoTemperatureControl"];
		fishStatus.relayStatus 				= root["relayStatus"];

		root.printTo(tempStr);
		Serial.print("Message received at topic " + topic + ": " + tempStr + "\n\n");	
	}

	if(topic == MQTT_TOPIC_SUB_CONFIG)
	{
		String tempStr;
		StaticJsonBuffer<500> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(message);

		fishConfig.temperature_a_coeficient		= root["temperature_a_coeficient"];
		fishConfig.temperature_b_coeficient 	= root["temperature_b_coeficient"];
		fishConfig.sendDataInterval				= root["sendDataInterval"];		

		root.printTo(tempStr);
		Serial.print("Message received at topic " + topic + ": " + tempStr + "\n\n");	

		procTimer.stop();
		procTimer.initializeMs(fishConfig.sendDataInterval * 1000, publishMessage).start(); // every 20 seconds
	}
}


void listNetworks(bool succeeded, BssList list)
{
	if (!succeeded)
	{
		Serial.println("\nFailed to scan networks. Scanning again");
		return;
	}

	for (int i = 0; i < list.count(); i++)
	{
		Serial.print("\tWiFi: ");
		Serial.print(list[i].ssid);
		Serial.print(", ");
		Serial.print(list[i].getAuthorizationMethodName());
		if (list[i].hidden) Serial.print(" (hidden)");
		Serial.println();
	}
}

// Will be called when WiFi station timeout was reached
void connectFail(String ssid, uint8_t ssid_len, uint8_t bssid[6], uint8_t reason)
{
	Serial.println("\n-- I'm NOT CONNECTED. Trying again --\n");

	WifiStation.startScan(listNetworks); // In Sming we can start network scan from init method without additional code
	reconnectTimer.initializeMs(5 * 1000, wifiConnect).start();
}

void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	// Run MQTT client

	Serial.println("-- WIFI CONNECTED. --");
	Serial.println("Fish's IP: " + ip.toString());
	startMqttClient();
}

void wifiConnect()
{
	reconnectTimer.stop();

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}

// Run MQTT client
void startMqttClient()
{
	procTimer.stop();

	if(!mqtt->setWill("last/will","The connection from this device is lost:(", 1, true)) {
		debugf("Unable to set the last will and testament. Most probably there is not enough memory on the device.");
	}
	mqtt->connect(MQTT_ID);

	// Assign a disconnect callback function
	mqtt->setCompleteDelegate(checkMQTTDisconnect);
	mqtt->subscribe(MQTT_TOPIC_SUB_DATA );
	mqtt->subscribe(MQTT_TOPIC_SUB_CONFIG );

	procTimer.initializeMs(fishConfig.sendDataInterval * 1000, publishMessage).start(); 
}

void loopTemperatureControl()
{

	updateFishData();

	if(fishStatus.autoTemperatureControl)
	{
		/*
		Serial.print("Set Point Temp: ");
		Serial.print(fishStatus.setPointTemperature);
		Serial.print("\nTemperature: ");	
		Serial.print(fishStatus.temperature);	
		Serial.print("\n\n");
		*/
		if(fishStatus.temperature > (fishStatus.setPointTemperature + fishStatus.deadBandTemperature))
		{	
			digitalWrite(RELAY_PIN,LOW);		
		}else if(fishStatus.temperature < (fishStatus.setPointTemperature - fishStatus.deadBandTemperature))
		{
			digitalWrite(RELAY_PIN,HIGH);		
		}

	}else
	{
		digitalWrite(RELAY_PIN,fishStatus.relayStatus);
	}
}

void ready()
{
	debugf("\n------ PROGRAM STARTED ------!\n");

	loopTemperatureControlTimer.initializeMs(500, loopTemperatureControl).start();
	mqtt = new MqttClient(MQTT_HOST, MQTT_PORT, onMessageReceived);

	wifiConnect();
}

void init()
{
	pinMode(LED_R_PIN, OUTPUT);
	pinMode(LED_G_PIN, OUTPUT);
	pinMode(LED_B_PIN, OUTPUT);
	pinMode(RELAY_PIN, OUTPUT);

	digitalWrite(LED_R_PIN,LOW);
	digitalWrite(LED_G_PIN,LOW);
	digitalWrite(LED_B_PIN,LOW);
	digitalWrite(RELAY_PIN,HIGH);

	fishConfig.temperature_a_coeficient = -0.078;
	fishConfig.temperature_b_coeficient = 83.1;
	fishConfig.sendDataInterval			= 10;
	fishStatus.autoTemperatureControl	= true;
	fishStatus.setPointTemperature		= 28;
	fishStatus.deadBandTemperature		= 0.5;

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Debug output to serial

	System.onReady(ready);
}
