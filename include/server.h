#ifndef SERVER_H_
#define SERVER_H_

#pragma message "DEBUG_server.h"

#include <SmingCore.h>
#include <AppSettings.h>


class c_localServer{
public:
	c_localServer();
	~c_localServer();
    
    HttpServer server;
};

#endif //SERVER_H_