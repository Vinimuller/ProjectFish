#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <string.h>


struct s_fishStatus fishStatus;

#define LED_R_PIN	15
#define LED_G_PIN	12 
#define LED_B_PIN	13
#define RELAY_PIN	14


// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "Virus.com" // Put you SSID and Password here
	#define WIFI_PWD "Um2tres4cinco"
#endif

// ... and/or MQTT username and password
#ifndef MQTT_USERNAME
	#define MQTT_USERNAME ""
	#define MQTT_PWD ""
#endif

#ifndef MQTT_TOPIC_SUBSCRIBE
	#define MQTT_TOPIC_SUBSCRIBE "U7886zhUcV_fish_house/setPointTemperature"
#endif

#ifndef MQTT_TOPIC_PUBLISH
	#define MQTT_TOPIC_PUBLISH "U7886zhUcV_fish_house/temperature"
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
void onMessageReceived(String topic, String message);

Timer procTimer;

// MQTT client
// For quick check you can use: http://www.hivemq.com/demos/websocket-client/ (Connection= test.mosquitto.org:8080)
MqttClient *mqtt;

//Get value from ADC and transform into Celsius
float getTemperature()
{
	int adc 				= system_adc_read();
	fishStatus.temperature 	= (adc);
	return fishStatus.temperature;
}

// Check for MQTT Disconnection
void checkMQTTDisconnect(TcpClient& client, bool flag){
	
	// Called whenever MQTT connection is failed.
	if (flag == true)
		Serial.println("MQTT Broker Disconnected!!");
	else
		Serial.println("MQTT Broker Unreachable!!");
	
	// Restart connection attempt after few seconds
	procTimer.initializeMs(2 * 1000, startMqttClient).start(); // every 2 seconds
}

void onMessageDelivered(uint16_t msgId, int type) {
	Serial.printf("Message with id %d and QoS %d was delivered successfully.\n", msgId, (type==MQTT_MSG_PUBREC? 2: 1));
}

// Publish our message
void publishMessage()
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	
	fishStatus.temperature 			= getTemperature();
	fishStatus.relayStatus			= digitalRead(RELAY_PIN); 

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
	String tempStr;
	StaticJsonBuffer<500> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(message);

	fishStatus.setPointTemperature		= root["setPointTemperature"];
	fishStatus.deadBandTemperature 		= root["deadBandTemperature"];		
	fishStatus.temperature 				= root["temperature"];
	fishStatus.autoTemperatureControl 	= root["autoTemperatureControl"];
	fishStatus.relayStatus 				= root["relayStatus"];

	root.printTo(tempStr);
	Serial.print("Message received at topic " + topic + ": " + tempStr + "\n");
/*
	if(topic == MQTT_TOPIC_SUBSCRIBE)
	{	
		//fishStatus.setPointTemperature = message.toInt();
		//tempStr = fishStatus.setPointTemperature;

		Serial.println("\n\n New temperature set point received: " + tempStr + " degrees\n\n");
				
		digitalWrite(LED_R_PIN,fishStatus.setPointTemperature);
			
	}
	*/
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
	mqtt->subscribe(MQTT_TOPIC_SUBSCRIBE);
}

// Will be called when WiFi station timeout was reached
void connectFail(String ssid, uint8_t ssid_len, uint8_t bssid[6], uint8_t reason)
{
	Serial.println("I'm NOT CONNECTED. Need help :(");

	// .. some you code for device configuration ..
}

void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	// Run MQTT client
	startMqttClient();

	// Start publishing loop
	procTimer.initializeMs(10 * 1000, publishMessage).start(); // every 20 seconds
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


	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Debug output to serial

	mqtt = new MqttClient(MQTT_HOST, MQTT_PORT, onMessageReceived);

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}
