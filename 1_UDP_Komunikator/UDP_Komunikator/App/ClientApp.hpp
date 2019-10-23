#ifndef ClientApp_hpp
#define ClientApp_hpp

#include <iostream>
#include <thread>
#include <string>
#include <sys/poll.h>
#include <unistd.h>
#include "../Network/IP.hpp"
#include "../Network/Port.hpp"
#include "../Network/ClientSocket.hpp"
#include "./App.hpp"

using namespace std;

class ClientApp : public App {
public:
	ClientApp(string directory);
	~ClientApp();
	void run();
	
private:
	IP getIP();
};

#endif
