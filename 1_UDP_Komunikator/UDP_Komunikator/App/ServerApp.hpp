#ifndef ServerApp_hpp
#define ServerApp_hpp

#include <iostream>
#include <string>
#include <thread>
#include <sys/poll.h>
#include "../Network/Port.hpp"
#include "../Network/ServerSocket.hpp"
#include "./App.hpp"

using namespace std;

class ServerApp : public App {
public:
	void run();
	
private:
	Port getPort();
};

#endif
