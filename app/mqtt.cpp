#include <mqtt.h>
#include <SmingCore/SmingCore.h>

MqttClient *mqtt;
Timer procTimer;
extern struct s_fishStatus fishStatus;
extern struct s_fishConfig fishConfig;

void onMessageReceived(String topic, String message);
void checkMQTTDisconnect(TcpClient& client, bool flag);
void publishMessage();
void onMessageDelivered(uint16_t msgId, int type);
extern void updateFishData();

// Run MQTT client
void startMqttClient()
{
	procTimer.stop();

	mqtt = new MqttClient(MQTT_HOST, MQTT_PORT, onMessageReceived);

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
	procTimer.initializeMs(2 * 1000, startMqttClient).start(); // every 2 seconds
}