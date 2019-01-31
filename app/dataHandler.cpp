#pragma message "DEBUG_DataHandler.c"

#include <dataHandler.h>

extern c_fish *fish;

HttpClient thingSpeak;

int onHttpDataSent(HttpConnection& client, bool successful);

c_dataHandler::c_dataHandler()
{
	Serial.println("\n=== DATA HANDLER CONSTRUCTOR ===");

    taskTimer.initializeMs(TASK_INTERVAL_DATA, TimerDelegate(&c_dataHandler::mainLoop, this)).start();
}

c_dataHandler::~c_dataHandler()
{
    Serial.println("\n=== DATA HANDLER DESCONSTRUCTOR ===");

}

void c_dataHandler::mainLoop()
{
    Serial.println("\n=== DATA HANDLER MAIN LOOP ===");

    s_fishStatus fishStatus;

    fishStatus = fish->getFishStatus();

    send(fishStatus);
}

String c_dataHandler::mountJsonString(s_fishStatus &fishStatus)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	root["api_key"]				= "2PZCZLNKD7SQN4JW";
	root["field1"]				= fishStatus.temperature;

	String str;
	root.printTo(str);

	return str;
}

void c_dataHandler::send(s_fishStatus &fishStatus)
{
	Serial.println("\n=== SEND SENSOR DATA ===");

	HttpRequest *request = new HttpRequest(URL("http://api.thingspeak.com/update.json"));

	request->setHeader("Content-Type", "application/json");
	request->setMethod(HTTP_POST);

	request->setBody(mountJsonString(fishStatus));
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