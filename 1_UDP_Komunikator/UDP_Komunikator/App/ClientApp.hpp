#ifndef ClientApp_hpp
#define ClientApp_hpp

#include <iostream>
#include <string>
#include "../Network/IP.hpp"
#include "../Network/Port.hpp"
#include "../Network/ClientSocket.hpp"

using namespace std;

class ClientApp {
public:
	void run();
	
private:
	IP getIP();
	Port getPort();
};

#endif
