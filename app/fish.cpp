#pragma message "DEBUG_fish.c"

#include <fish.h>

c_fish::c_fish()
{
	Serial.println("\n=== FISH CONSTRUCTOR ===");
	fishInit();
}

c_fish::~c_fish()
{

}

void c_fish::fishInit()
{
    Serial.println("\n=== FISH INIT ===");

	pinMode(LED_R_PIN, OUTPUT);
	pinMode(LED_G_PIN, OUTPUT);
	pinMode(LED_B_PIN, OUTPUT);
	pinMode(RELAY_0_PIN, OUTPUT);
    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);

	digitalWrite(LED_R_PIN,LOW);
	digitalWrite(LED_G_PIN,LOW);
	digitalWrite(LED_B_PIN,LOW);
	digitalWrite(RELAY_0_PIN,HIGH);
    digitalWrite(RELAY_1_PIN,LOW);
    digitalWrite(RELAY_2_PIN,LOW);

	loadDefaultConfig();

    taskTimer.initializeMs(TASK_INTERVAL, TimerDelegate(&c_fish::loopTempControl, this)).start();
}
void c_fish::loopTempControl()
{
    Serial.println("\n=== LOOP TEMP CONTROL ===");

    updateFishData();

	if(fishConfig.autoTemperatureControl)
	{
		if(fishStatus.temperature > (fishConfig.setPointTemperature + fishConfig.upperDeadBandTemperature))
		{	
			digitalWrite(RELAY_0_PIN,LOW);		
		}else if(fishStatus.temperature < (fishConfig.setPointTemperature - fishConfig.lowerDeadBandTemperature))
		{
			digitalWrite(RELAY_0_PIN,HIGH);		
		}

	}else
	{
		digitalWrite(RELAY_0_PIN,fishConfig.relayStatus);
	}

	if(fishConfig.leds)
	{
		digitalWrite(LED_B_PIN,fishConfig.relayStatus);
	}
}

void c_fish::updateFishData()
{
    Serial.println("\n=== UPDATE FISH DATA ===");

    fishStatus.temperature 		= getTemperature();
	fishConfig.relayStatus		= digitalRead(RELAY_0_PIN); 

    Serial.printf("\tTemp: %f \n\r\tRelay: %d",fishStatus.temperature, fishConfig.relayStatus);
}

void c_fish::loadDefaultConfig()
{
    Serial.println("\n=== LOAD DEFAULT CONFIG ===");

    s_fishConfig fishConfig;

    fishConfig.temperature_a_coeficient = -0.078;
	fishConfig.temperature_b_coeficient = 83.1;
	fishConfig.sendDataInterval			= 10;
	fishConfig.autoTemperatureControl	= true;
	fishConfig.setPointTemperature		= 28;
	fishConfig.upperDeadBandTemperature	= 1;
	fishConfig.lowerDeadBandTemperature = 1;
	fishConfig.relayStatus				= false;
	fishConfig.leds						= true;

    setFishConfig(fishConfig);
}

s_fishStatus c_fish::getFishStatus()
{
	return fishStatus;
}

float c_fish::getTemperature()
{
    Serial.println("\n=== GET TEMPERATURE ===");

    int adc = 0, i = 0;
	
	for (i = 0; i < 100; i++)
	{
		adc += analogRead(A0);	
	}
	adc /= 100;

	return adc;
	//return (fishConfig.temperature_a_coeficient*adc + fishConfig.temperature_b_coeficient);
}

void c_fish::setFishConfig(s_fishConfig &fishConfig)
{
    Serial.println("\n=== SET FISH CONFIG ===");

    this->fishConfig.sendDataInterval			= fishConfig.sendDataInterval;
	this->fishConfig.autoTemperatureControl	    = fishConfig.autoTemperatureControl;
	this->fishConfig.setPointTemperature		= fishConfig.setPointTemperature;
	this->fishConfig.upperDeadBandTemperature	= fishConfig.upperDeadBandTemperature;
	this->fishConfig.lowerDeadBandTemperature   = fishConfig.lowerDeadBandTemperature;
	this->fishConfig.relayStatus				= fishConfig.relayStatus;
	this->fishConfig.leds						= fishConfig.leds;

    printFishConfig();
}

void c_fish::getFishConfig(s_fishConfig &fishConfig)
{
    Serial.println("\n=== GET FISH CONFIG ===");

    fishConfig.sendDataInterval			    = this->fishConfig.sendDataInterval;
	fishConfig.autoTemperatureControl	    = this->fishConfig.autoTemperatureControl;
	fishConfig.setPointTemperature		    = this->fishConfig.setPointTemperature;
	fishConfig.upperDeadBandTemperature	    = this->fishConfig.upperDeadBandTemperature;
	fishConfig.lowerDeadBandTemperature     = this->fishConfig.lowerDeadBandTemperature;
	fishConfig.relayStatus				    = this->fishConfig.relayStatus;
	fishConfig.leds						    = this->fishConfig.leds;
}

void c_fish::printFishConfig()
{
    Serial.println("\n=== PRINT FISH CONFIG ===");
    Serial.printf("\n Fish Config: \n\r");
    Serial.printf("\t\t sendDataInterval: %d\n\r",               fishConfig.sendDataInterval);
    Serial.printf("\t\t autoTemperatureControl: %d\n\r",         fishConfig.autoTemperatureControl);
    Serial.printf("\t\t setPointTemperature: %d\n\r",            fishConfig.setPointTemperature);
    Serial.printf("\t\t upperDeadBandTemperature: %f\n\r",       fishConfig.upperDeadBandTemperature);
    Serial.printf("\t\t lowerDeadBandTemperature: %f\n\r",       fishConfig.lowerDeadBandTemperature);
    Serial.printf("\t\t relayStatus: %d\n\r",                    fishConfig.relayStatus);
    Serial.printf("\t\t leds: %d\n\r",                           fishConfig.leds);
}