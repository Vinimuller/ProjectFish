/*
 * AppSettings.h
 *
 *  Created on: 13 ��� 2015 �.
 *      Author: Anakod
 */
#ifndef INCLUDE_APPSETTINGS_H_
#define INCLUDE_APPSETTINGS_H_

#pragma message "DEBUG_appSettings.h"

//#include <SmingCore.h>
#include <fish.h>

#define APP_SETTINGS_FILE ".settings.conf" // leading point for security reasons :)

struct ApplicationSettingsStorage 
{
	//Network config
	String ssid;
	String password;

	//Fish config
	s_fishConfig fishConfig;

	void load()
	{
		DynamicJsonBuffer jsonBuffer;
		if(exist()) 
		{
			int size = fileGetSize(APP_SETTINGS_FILE);
			char* jsonString 	= new char[size + 1];
			fileGetContent(APP_SETTINGS_FILE, jsonString, size + 1);

			JsonObject& root 	= jsonBuffer.parseObject(jsonString);

			JsonObject& network = root["network"];
			ssid 				= network["ssid"].asString();
			password 			= network["password"].asString();

			JsonObject& fish = root["fish"];
			fishConfig.setPointTemperature 		= fish["setPointTemperature"];
			fishConfig.upperDeadBandTemperature = fish["upperDeadBandTemperature"];
			fishConfig.lowerDeadBandTemperature = fish["lowerDeadBandTemperature"];
			fishConfig.autoTemperatureControl 	= fish["autoTemperatureControl"];
			fishConfig.relayStatus				= fish["relayStatus"];
			fishConfig.leds 					= fish["leds"];
			fishConfig.sendDataInterval 		= fish["sendDataInterval"];

			delete[] jsonString;
		}
	}

	void save()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root 	= jsonBuffer.createObject();

		JsonObject& network = jsonBuffer.createObject();
		root["network"] 	= network;
		network["ssid"] 	= ssid.c_str();
		network["password"] = password.c_str();

		JsonObject& fish = jsonBuffer.createObject();
		root["fish"] 					 = fish;
		fish["setPointTemperature"] 	 = fishConfig.setPointTemperature;
		fish["upperDeadBandTemperature"] = fishConfig.upperDeadBandTemperature;
		fish["lowerDeadBandTemperature"] = fishConfig.lowerDeadBandTemperature;
		fish["autoTemperatureControl"] 	 = fishConfig.autoTemperatureControl;
		fish["relayStatus"] 			 = fishConfig.relayStatus;
		fish["leds"] 					 = fishConfig.leds;
		fish["sendDataInterval"] 		 = fishConfig.sendDataInterval;

		//TODO: add direct file stream writing
		String rootString;
		root.printTo(rootString);
		fileSetContent(APP_SETTINGS_FILE, rootString);
	}

	bool exist()
	{
		return fileExist(APP_SETTINGS_FILE);
	}
};

static ApplicationSettingsStorage AppSettings;

#endif /* INCLUDE_APPSETTINGS_H_ */
