#ifndef __USER_MQTT_H__
#define __USER_MQTT_H__

#include <user_config.h>

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

#endif