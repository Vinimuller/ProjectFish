#ifndef FISH_H_
#define FISH_H_

#pragma message "DEBUG_fish.h"

#include <SmingCore.h>

#define LED_R_PIN   16
#define LED_G_PIN   14
#define LED_B_PIN   12

#define RELAY_0_PIN 4
#define RELAY_1_PIN 5
#define RELAY_2_PIN 13

#define TASK_INTERVAL 10*1000

typedef struct s_fishStatus
{
	float 	temperature;
	float 	airTemperature;
	float 	airHumidity;
} s_fishStatus;

typedef struct s_fishConfig
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
} s_fishConfig;

class c_fish{
public:
	c_fish();
	~c_fish();

    void fishInit();
    void loopTempControl();
    void updateFishData();
    void loadDefaultConfig();
    void setFishConfig(s_fishConfig &fishConfig);
    void getFishConfig(s_fishConfig &fishConfig);
	void printFishConfig();
    float getTemperature();
	s_fishStatus getFishStatus();

private:
    s_fishConfig fishConfig;
    s_fishStatus fishStatus;
    
    Timer taskTimer;
};

#endif //SERVER_H_