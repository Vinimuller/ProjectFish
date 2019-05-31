#pragma message "DEBUG_DataHandler.c"

#include <dataHandler.h>

extern c_fish *fish;

HttpClient thingSpeak;

int onHttpDataSent(HttpConnection& client, bool successful);

c_dataHandler::c_dataHandler()
{
	Serial.println("\n=== DATA HANDLER CONSTRUCTOR ===");

	setSendInterval(TASK_INTERVAL_DATA);
}

c_dataHandler::~c_dataHandler()
{
    Serial.println("\n=== DATA HANDLER DESCONSTRUCTOR ===");

}

void c_dataHandler::mainLoop()
{
    Serial.println("\n=== DATA HANDLER MAIN LOOP ===");

    s_fishStatus fishStatus;
	s_fishConfig fishConfig;

    fishStatus = fish->getFishStatus();
	fishConfig = fish->getFishConfig();

    send(fishStatus, fishConfig);
}

String c_dataHandler::mountJsonString(s_fishStatus &fishStatus, s_fishConfig &fishConfig)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	root["api_key"]				= "2PZCZLNKD7SQN4JW";
	root["field1"]				= fishStatus.temperature;
	root["field2"]				= fishStatus.relayStatus;
	root["field3"]				= fishConfig.upperTemperature;
	root["field4"]				= fishConfig.lowerTemperature;
	root["field5"]				= fishStatus.tOn;

	String str;
	root.printTo(str);

	return str;
}

void c_dataHandler::send(s_fishStatus &fishStatus, s_fishConfig &fishConfig)
{
	Serial.println("\n=== SEND SENSOR DATA ===");

	HttpRequest *request = new HttpRequest(URL("http://api.thingspeak.com/update.json"));

	request->setHeader("Content-Type", "application/json");
	request->setMethod(HTTP_POST);

	request->setBody(mountJsonString(fishStatus, fishConfig));
	request->onRequestComplete(RequestCompletedDelegate(onHttpDataSent));

	if(!thingSpeak.send(request))
	{
		thingSpeak.freeRequestQueue();
		Serial.println("\n\t== HttpRequest Request Queue freed ==\n");
	}else
    {
        digitalWrite(LED_B_PIN,1);
    }
    
}

void c_dataHandler::setSendInterval(int interval)
{
	taskTimer.initializeMs(interval, TimerDelegate(&c_dataHandler::mainLoop, this)).start();	
}

int onHttpDataSent(HttpConnection& client, bool successful)
{
	Serial.println("\n=== SERVER RESPONSE ===");

    String response = client.getResponseString();

	if (successful)
    {
        Serial.println("Succes");
        if(response.length() > 10)
            digitalWrite(LED_B_PIN,0);
    }
	else
		Serial.println("Failed");
    
	Serial.println("Server response: '" + response + "'");
	return 0;
}