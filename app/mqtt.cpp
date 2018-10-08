#include <mqtt.h>
#include <SmingCore/SmingCore.h>


MqttClient *mqtt;
Timer procTimer;

extern s_fishStatus fishStatus;
extern s_fishConfig fishConfig;

void onMessageDelivered(uint16_t msgId, int type);
void onMessageReceived(String topic, String message);
void checkMQTTDisconnect(TcpClient& client, bool flag);

// Run MQTT client
void startMqttClient()
{
	mqtt = new MqttClient(MQTT_HOST, MQTT_PORT, onMessageReceived);

	if(!mqtt->setWill("last/will","The connection from this device is lost:(", 1, true)) {
		debugf("Unable to set the last will and testament. Most probably there is not enough memory on the device.");
	}
	mqtt->connect(MQTT_ID);
	
	// Assign a disconnect callback function
	mqtt->setCompleteDelegate(checkMQTTDisconnect);
	mqtt->subscribe(MQTT_TOPIC_SUB1);
}

// Callback for messages, arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	String tempStr;
	StaticJsonBuffer<500> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(message);
	
	fishConfig.setPointTemperature			= root["setPointTemperature"];
	fishConfig.upperDeadBandTemperature		= root["upperDeadBandTemperature"];
	fishConfig.lowerDeadBandTemperature		= root["lowerDeadBandTemperature"];
	fishConfig.temperature_a_coeficient		= root["temperature_a_coeficient"];
	fishConfig.temperature_b_coeficient		= root["temperature_b_coeficient"];
	fishConfig.autoTemperatureControl		= root["autoTemperatureControl"];
	fishConfig.relayStatus					= root["relayStatus"];
	fishConfig.sendDataInterval				= root["sendDataInterval"];
	fishConfig.leds							= root["leds"];
	
	root.printTo(tempStr);
	Serial.print("Message received at topic " + topic + ": " + tempStr + "\n\n");	
}

// Publish our message
void publishMessage()
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root1 = jsonBuffer.createObject();
	JsonObject& root2 = jsonBuffer.createObject();
	JsonObject& root3 = jsonBuffer.createObject();
	JsonObject& root4 = jsonBuffer.createObject();

	root1["fishTemperature"]		= fishStatus.temperature;
	
	root2["airTemperature"]			= fishStatus.airTemperature;
	
	root3["airHumidity"]			= fishStatus.airHumidity;
	
	root4["setPointTemperature"]		= fishConfig.setPointTemperature;
	root4["upperDeadBandTemperature"]	= fishConfig.upperDeadBandTemperature;
	root4["lowerDeadBandTemperature"]	= fishConfig.lowerDeadBandTemperature;
	root4["temperature_a_coeficient"]	= fishConfig.temperature_a_coeficient;
	root4["temperature_b_coeficient"]	= fishConfig.temperature_b_coeficient;
	root4["autoTemperatureControl"]		= fishConfig.autoTemperatureControl;
	root4["relayStatus"]				= fishConfig.relayStatus;
	root4["sendDataInterval"]			= fishConfig.sendDataInterval;
	root4["leds"]						= fishConfig.leds;
	
	String fishTempStr, airTempStr, airHumStr, configStr;
	root1.printTo(fishTempStr);
	root2.printTo(airTempStr);
	root3.printTo(airHumStr);
	root4.printTo(configStr);

	if (mqtt->getConnectionState() != eTCS_Connected)
		startMqttClient(); // Auto reconnect

	mqtt->publishWithQoS(MQTT_TOPIC_PUB1,fishTempStr,1,false,onMessageDelivered); 
	//mqtt->publishWithQoS(MQTT_TOPIC_PUB2,airTempStr,1,false,onMessageDelivered); 
	//mqtt->publishWithQoS(MQTT_TOPIC_PUB3,airHumStr,1,false,onMessageDelivered); 
	//mqtt->publishWithQoS(MQTT_TOPIC_PUB4,configStr,1,false,onMessageDelivered); 
}

void onMessageDelivered(uint16_t msgId, int type)
{
	Serial.printf("Message with id %d and QoS %d was delivered successfully.\n\n", msgId, (type==MQTT_MSG_PUBREC? 2: 1));
}

// Check for MQTT Disconnection
void checkMQTTDisconnect(TcpClient& client, bool flag)
{
	// Called whenever MQTT connection is failed.
	if (flag == true)
		Serial.println("MQTT Broker Disconnected!!\n\n");
	else
		Serial.println("MQTT Broker Unreachable!!\n\n");
	
	// Restart connection attempt after few seconds
	procTimer.initializeMs(5 * 1000, startMqttClient).start(); // every 2 seconds
}