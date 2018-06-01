#ifndef __USER_MQTT_H__
#define __USER_MQTT_H__

	#include <user_config.h>
	
	#define MQTT_USERNAME ""
	#define MQTT_PWD ""

	#define MQTT_TOPIC_SUB1 		"U7886zhUcV_fish_house/config/rx"

	#define MQTT_TOPIC_PUB1 		"U7886zhUcV_fish_house/data/fishtemp"
	#define MQTT_TOPIC_PUB2 		"U7886zhUcV_fish_house/data/airtemp"
	#define MQTT_TOPIC_PUB3 		"U7886zhUcV_fish_house/data/airhumidity"
	#define MQTT_TOPIC_PUB4 		"U7886zhUcV_fish_house/config/tx"

	#define MQTT_ID 				"ESP_Fish"
	#define MQTT_HOST 				"iot.eclipse.org"
	#define MQTT_PORT 				1883
#endif
