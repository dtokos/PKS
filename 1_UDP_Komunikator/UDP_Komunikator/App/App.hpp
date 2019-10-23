#ifndef App_hpp
#define App_hpp

#include <unistd.h>
#include <fstream>
#include <exception>
#include "../Network/Socket.hpp"
#include "../Network/Port.hpp"
#include "Message.hpp"

class App {
public:
	App(string directory);
	virtual void run() = 0;
	
protected:
	struct AppNotOpenError : public exception {
		const char * what () const throw () {
			return "Cannot read/write while app is closed";
		}
	};
	
	string directory;
	thread readingThread;
	bool isOpen = true;
	bool didDisconnect = false;
	
	Port getPort();
	size_t getMaxSegmentSize();
	void comunicate(Socket &socket);
	void printMenu();
	void readLoop(Socket &socket);
	Message::Type receiveMessageType(Socket &socket);
	void receiveTextMessage(Socket &socket);
	void receiveFileMessage(Socket &socket);
	void receiveBytes(Socket &socket, char *buffer, uint32_t count);
	void writeLoop(Socket &socket);
	void sendTextMessage(Socket &socket);
	void writeTextMessage(Socket &socket, const Message::Text &message);
	void sendFileMessage(Socket &socket);
	string getFileName(string &path);
	void writeFileMessage(Socket &socket, const Message::File &message, ifstream &file);
	void readUserInput(string &buffer);
};

#endif
