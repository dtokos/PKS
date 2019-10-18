#ifndef ServerApp_hpp
#define ServerApp_hpp

#include <iostream>
#include <string>
#include <sys/poll.h>
#include "../Network/Port.hpp"
#include "../Network/ServerSocket.hpp"

using namespace std;

class ServerApp {
public:
	void run();
	
private:
	Port getPort();
};

#endif
