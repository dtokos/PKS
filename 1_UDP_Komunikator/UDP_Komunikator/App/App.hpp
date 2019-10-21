#ifndef App_hpp
#define App_hpp

#include "../Network/Socket.hpp"

class App {
	virtual void run() = 0;
	
protected:
	thread readingThread;
	bool isOpen = true;
	void readFrom(Socket &socket);
	void printMenu();
};

#endif
