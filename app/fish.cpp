#include <fish.h>
#include <SmingCore/SmingCore.h>

extern s_fishStatus fishStatus;
extern s_fishConfig fishConfig;

void fishInit()
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
	fishConfig.autoTemperatureControl	= true;
	fishConfig.setPointTemperature		= 28;
	fishConfig.upperDeadBandTemperature	= 0.5;
	fishConfig.lowerDeadBandTemperature = 1;
	fishConfig.relayStatus				= false;
	fishConfig.leds						= true;
}

//Get value from ADC and transform into Celsius
float getTemperature()
{
	int adc = 0;

	adc = analogRead(A0);	

	return (fishConfig.temperature_a_coeficient*adc + fishConfig.temperature_b_coeficient);
}

//Get sensors data
void updateFishData()
{
	fishStatus.temperature 		= getTemperature();
	fishConfig.relayStatus		= digitalRead(RELAY_PIN); 
}

//Main loop for temperature control
void loopTemperatureControl()
{
	if(fishConfig.autoTemperatureControl)
	{
		if(fishStatus.temperature > (fishConfig.setPointTemperature + fishConfig.upperDeadBandTemperature))
		{	
			digitalWrite(RELAY_PIN,LOW);		
		}else if(fishStatus.temperature < (fishConfig.setPointTemperature - fishConfig.lowerDeadBandTemperature))
		{
			digitalWrite(RELAY_PIN,HIGH);		
		}

	}else
	{
		digitalWrite(RELAY_PIN,fishConfig.relayStatus);
	}

	if(fishConfig.leds)
	{
		digitalWrite(LED_R_PIN,fishConfig.relayStatus);
	}
}