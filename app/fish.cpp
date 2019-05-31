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
	
	float lastTemp = fishStatus.temperature;
	fishStatus.tOnCnt++;
	
    updateFishData();

	if(lastTemp <= fishConfig.upperTemperature && fishStatus.temperature > fishConfig.upperTemperature)
	{
		fishStatus.tOn = fishStatus.tOnCnt/120;
		fishStatus.tOnCnt = 0;
	}

	if(fishConfig.autoTemperatureControl)
	{
		if(fishStatus.temperature > fishConfig.upperTemperature)
		{	
			digitalWrite(RELAY_0_PIN,LOW);		
		}else if(fishStatus.temperature < fishConfig.lowerTemperature)
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
	}else
	{
		digitalWrite(LED_R_PIN,0);
		digitalWrite(LED_G_PIN,0);
		digitalWrite(LED_B_PIN,0);
	}
}

void c_fish::updateFishData()
{
    Serial.println("\n=== UPDATE FISH DATA ===");

    fishStatus.temperature 		= getTemperature();
	fishStatus.relayStatus		= digitalRead(RELAY_0_PIN); 
	fishConfig.relayStatus		= digitalRead(RELAY_0_PIN); 

    Serial.printf("\tTemp: %.1f \n\r\tRelay: %d",fishStatus.temperature, fishConfig.relayStatus);
}

void c_fish::loadDefaultConfig()
{
    Serial.println("\n=== LOAD DEFAULT CONFIG ===");

    s_fishConfig fishConfig;

    fishConfig.temperature_a_coeficient = -0.0794;
	fishConfig.temperature_b_coeficient = 83.5;
	fishConfig.sendDataInterval			= 10;
	fishConfig.autoTemperatureControl	= true;
	fishConfig.upperTemperature			= 28.8;
	fishConfig.lowerTemperature 		= 27.2;
	fishConfig.relayStatus				= false;
	fishConfig.leds						= true;
	fishStatus.tOn						= 0;

    setFishConfig(fishConfig);
}

s_fishStatus c_fish::getFishStatus()
{
	return fishStatus;
}

s_fishConfig c_fish::getFishConfig()
{
	return fishConfig;
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

	//return ((float)fishConfig.temperature_a_coeficient*(float)adc + fishConfig.temperature_b_coeficient);
	Serial.printf("\n\tAdc: %d",adc);
	return ((float)-0.0825*(float)adc + 86.6);
}

void c_fish::setFishConfig(s_fishConfig &fishConfig)
{
    Serial.println("\n=== SET FISH CONFIG ===");

    this->fishConfig.sendDataInterval			= fishConfig.sendDataInterval;
	this->fishConfig.upperTemperature			= fishConfig.upperTemperature;
	this->fishConfig.lowerTemperature   		= fishConfig.lowerTemperature;
	this->fishConfig.leds						= fishConfig.leds;

    printFishConfig();
}

void c_fish::getFishConfig(s_fishConfig &fishConfig)
{
    Serial.println("\n=== GET FISH CONFIG ===");

    fishConfig.sendDataInterval			    = this->fishConfig.sendDataInterval;
	fishConfig.autoTemperatureControl	    = this->fishConfig.autoTemperatureControl;
	fishConfig.upperTemperature	   			= this->fishConfig.upperTemperature;
	fishConfig.lowerTemperature     		= this->fishConfig.lowerTemperature;
	fishConfig.relayStatus				    = this->fishConfig.relayStatus;
	fishConfig.leds						    = this->fishConfig.leds;
}


void c_fish::printFishConfig()
{
    Serial.println("\n=== PRINT FISH CONFIG ===");
    Serial.printf("\n Fish Config: \n\r");
    Serial.printf("\t\t sendDataInterval: %d\n\r",               fishConfig.sendDataInterval);
    Serial.printf("\t\t autoTemperatureControl: %d\n\r",         fishConfig.autoTemperatureControl);
    Serial.printf("\t\t upperTemperature: %f\n\r",       		 fishConfig.upperTemperature);
    Serial.printf("\t\t lowerTemperature: %f\n\r",      		 fishConfig.lowerTemperature);
    Serial.printf("\t\t relayStatus: %d\n\r",                    fishConfig.relayStatus);
    Serial.printf("\t\t leds: %d\n\r",                           fishConfig.leds);
}