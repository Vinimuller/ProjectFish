#pragma message "DEBUG_Server.c"

#include <server.h>

void onIndex(HttpRequest& request, HttpResponse& response);
void onFile(HttpRequest& request, HttpResponse& response);
void onChangeWifi(HttpRequest& request, HttpResponse& response);

c_localServer::c_localServer()
{
	Serial.println("\n=== SERVER CONSTRUCTOR ===");
	
    server.listen(80);
	server.addPath("/", onIndex);
    server.addPath("/changeWifi", onChangeWifi);
	server.setDefaultHandler(onFile);
}

c_localServer::~c_localServer()
{

}

void onChangeWifi(HttpRequest& request, HttpResponse& response)
{
    Serial.println("\n=== ON CHANGE WIFI ===");
    
    AppSettings.ssid     = request.getPostParameter("SSID");
    AppSettings.password = request.getPostParameter("PWD");
    AppSettings.save();
    
    Serial.printf("Connecting to SSID: %s PWD: %s \n",AppSettings.ssid.c_str(),AppSettings.password.c_str());

    WifiStation.config(AppSettings.ssid, AppSettings.password);
    WifiStation.connect();

    response.sendFile("message.html");
}

void onIndex(HttpRequest& request, HttpResponse& response)
{
    Serial.println("\n=== ON INDEX ===");
    response.setCache(86400, true); // It's important to use cache for better performance.
	response.sendFile("index.html"); // will be automatically deleted
}

void onFile(HttpRequest& request, HttpResponse& response)
{
	String file = request.getPath();

	if(file[0] == '/')
    {
		file = file.substring(1);
    }

	if(file[0] == '.')
    {
		response.forbidden();
    }
	else 
    {
        if(fileExist(file))
        {
            response.setCache(86400, true); // It's important to use cache for better performance.
            response.sendFile(file);
        }else
        {
            Serial.printf("\nrequested file doesn't exist: %s\n",file.c_str());
        }
  
	}
}