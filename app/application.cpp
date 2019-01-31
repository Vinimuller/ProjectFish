#include <user_config.h>
#include <SmingCore.h>
#include <server.h>
#include <fish.h>
#include <dataHandler.h>
#include <AppSettings.h>

c_localServer *localServer = NULL;
c_fish 		  *fish		   = NULL;
c_dataHandler *dataHandler = NULL;

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

	WifiStation.startScan(listNetworks); 
}

void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	Serial.println("-- WIFI CONNECTED. --");
	Serial.println("Fish's IP: " + ip.toString());

}

void ready()
{
	debugf("\n------ PROGRAM STARTED ------!\n");

	localServer = new c_localServer();
	fish 		= new c_fish();
	dataHandler = new c_dataHandler();
	
	//Load settings
	AppSettings.load();
	fish->getFishConfig(AppSettings.fishConfig);

	//Start Access Point
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("Fish Peaks", "", AUTH_OPEN);
	WifiAccessPoint.setIP(IPAddress(192,168,45,1));

	WifiStation.enable(true);
	WifiStation.config("raks", "13092017");

	//load Settings
	if(AppSettings.exist()) 
	{
		WifiStation.config(AppSettings.ssid, AppSettings.password);
		fish->setFishConfig(AppSettings.fishConfig);
	}

	WifiStation.connect();

	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}

void init()
{
	spiffs_mount();

	//Serial config
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.begin(115200);
	Serial.systemDebugOutput(true); // Allow debug print to serial

	//System config
	SystemClock.setTimeZone(-3); // GMT-3

	System.setCpuFrequency(eCF_160MHz);
	Serial.print("New CPU frequency is: ");
	Serial.println((int)System.getCpuFrequency());

	System.onReady(ready);
}
