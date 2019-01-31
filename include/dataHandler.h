#ifndef DATA_HANDLER_H_
#define DATA_HANDLER_H_

#pragma message "DEBUG_DataHandler.h"

#include <SmingCore.h>
#include <fish.h>
#include <AppSettings.h>

#define MQTT_URL			"mqtt://mqtt.thingspeak.com:1883"
#define MQTT_USER			"fish"
#define MQTT_PASSWORD

#define CENTRAL_ID			""

#define MQTT_TOPIC_SENSOR 	"channels/692056/publish/fields/field1/2PZCZLNKD7SQN4JW"

#define TASK_INTERVAL 10*1000

class c_dataHandler{
public:
	c_dataHandler();
	~c_dataHandler();

    
    String mountJsonString(s_fishStatus &fishStatus);
    void send(s_fishStatus &fishStatus);
    void mainLoop();

private:    
    Timer taskTimer;
    
};

#endif //DATA_HANDLER_H_