#ifndef __USER_FISH_H__
#define __USER_FISH_H__

#include <user_config.h>

#define RELAY_PIN	15
#define LED_R_PIN	15
#define LED_G_PIN	12 
#define LED_B_PIN	13

struct s_fishStatus{
	int 	setPointTemperature;
	float 	deadBandTemperature;
	float 	temperature;
	bool 	autoTemperatureControl;
	bool 	relayStatus;
};

struct s_fishConfig{
	float temperature_a_coeficient;
	float temperature_b_coeficient;
	int	  sendDataInterval;
};

/*
void fishInit();
float getTemperature();
void updateFishData();
void loopTemperatureControl();
*/
#endif