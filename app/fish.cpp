#include <fish.h>
#include <SmingCore/SmingCore.h>

Timer loopTemperatureControlTimer;
struct s_fishStatus fishStatus;
struct s_fishConfig fishConfig;

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
	fishStatus.autoTemperatureControl	= true;
	fishStatus.setPointTemperature		= 28;
	fishStatus.deadBandTemperature		= 0.5;

	loopTemperatureControlTimer.initializeMs(500, loopTemperatureControl).start();
}

//Get value from ADC and transform into Celsius
float getTemperature()
{
	int adc = 0;

	adc = analogRead(A0);	

	return fishConfig.temperature_a_coeficient*adc + fishConfig.temperature_b_coeficient;
}

//Get sensors data
void updateFishData()
{
	fishStatus.temperature 			= getTemperature();
	fishStatus.relayStatus			= digitalRead(RELAY_PIN); 
}

//Main loop for temperature control
void loopTemperatureControl()
{
	updateFishData();

	if(fishStatus.autoTemperatureControl)
	{
		/*
		Serial.print("Set Point Temp: ");
		Serial.print(fishStatus.setPointTemperature);
		Serial.print("\nTemperature: ");	
		Serial.print(fishStatus.temperature);	
		Serial.print("\n\n");
		*/
		if(fishStatus.temperature > (fishStatus.setPointTemperature + fishStatus.deadBandTemperature))
		{	
			digitalWrite(RELAY_PIN,LOW);		
		}else if(fishStatus.temperature < (fishStatus.setPointTemperature - fishStatus.deadBandTemperature))
		{
			digitalWrite(RELAY_PIN,HIGH);		
		}

	}else
	{
		digitalWrite(RELAY_PIN,fishStatus.relayStatus);
	}
}