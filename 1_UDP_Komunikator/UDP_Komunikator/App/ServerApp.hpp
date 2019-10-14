#ifndef ServerApp_hpp
#define ServerApp_hpp

#include <iostream>
#include "../Network/Port.hpp"

using namespace std;

class ServerApp {
public:
	void run();
	
private:
	Port getPort();
};

#endif
