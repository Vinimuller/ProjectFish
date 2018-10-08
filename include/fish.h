#ifndef __USER_FISH_H__
#define __USER_FISH_H__

#include <user_config.h>

#define RELAY_PIN	5
#define LED_R_PIN	5
#define LED_G_PIN	5 
#define LED_B_PIN	5

struct s_fishStatus
{
	float 	temperature;
	float 	airTemperature;
	float 	airHumidity;
};

struct s_fishConfig
{
	int 	setPointTemperature;
	float 	upperDeadBandTemperature;
	float 	lowerDeadBandTemperature;
	float 	temperature_a_coeficient;
	float 	temperature_b_coeficient;
	bool 	autoTemperatureControl;
	bool 	relayStatus;
	bool	leds;
	int	  	sendDataInterval;
};

#endif