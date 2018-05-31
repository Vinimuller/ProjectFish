#include <user_config.h>
#include <SmingCore/SmingCore.h>


// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "Virus.com" // Put you SSID and Password here
	#define WIFI_PWD "Um2tres4cinco"
#endif

Timer reconnectTimer;
void wifiConnect();
extern void startMqttClient();

void listNetworks(bool succeeded, BssList list)
{
	if (!succeeded)
	{
		Serial.println("\nFailed to scan networks. Scanning again");
		return;
	}

	for (int i = 0; i < list.count(); i++)
	{
		Serial.print("\tWiFi: ");
		Serial.print(list[i].ssid);
		Serial.print(", ");
		Serial.print(list[i].getAuthorizationMethodName());
		if (list[i].hidden) Serial.print(" (hidden)");
		Serial.println();
	}
}

// Will be called when WiFi station timeout was reached
void connectFail(String ssid, uint8_t ssid_len, uint8_t bssid[6], uint8_t reason)
{
	Serial.println("\n-- I'm NOT CONNECTED. Trying again --\n");

	WifiStation.startScan(listNetworks); // In Sming we can start network scan from init method without additional code
	reconnectTimer.initializeMs(5 * 1000, wifiConnect).start();
}

void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	// Run MQTT client

	Serial.println("-- WIFI CONNECTED. --");
	Serial.println("Fish's IP: " + ip.toString());
	startMqttClient();
}

void wifiConnect()
{
	reconnectTimer.stop();

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}


void ready()
{
	debugf("\n------ PROGRAM STARTED ------!\n");

	wifiConnect();
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Debug output to serial

	System.onReady(ready);
}
