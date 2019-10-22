#include "App.hpp"

void App::comunicate(Socket &socket) {
	try {
		readingThread = thread(&App::readLoop, this, ref(socket));
		writeLoop(socket);
	} catch (Socket::SocketWriteError e) {
		cerr << "[APP][ERR] Could not write data to socket" << endl;
		cerr << "[RDP][ERR] " << e.what() << endl;
	}
	
	isOpen = false;
	socket.disconnect();
	readingThread.join();
}

void App::readLoop(Socket &socket) {
	try {
		while (isOpen) {
			Message::Type type = receiveMessageType(socket);
			
			switch (type) {
				case Message::Type::TextMessage:
					receiveTextMessage(socket);
					break;
					
				case Message::Type::FileMessage:
					receiveFileMessage(socket);
					break;
					
				default:
					break;
			}
		}
	} catch (AppNotOpenError e) {}
}

Message::Type App::receiveMessageType(Socket &socket) {
	Message::Type type;
	receiveBytes(socket, (char *)&type, sizeof(type));
	
	return type;
}

void App::receiveTextMessage(Socket &socket) {
	uint8_t length;
	receiveBytes(socket, (char *)&length, sizeof(length));
	
	char message[length + 1];
	receiveBytes(socket, message, length);
	message[length] = '\0';
	
	cout << "Received message: " << message << endl;
}

void App::receiveFileMessage(Socket &socket) {
	uint8_t nameLength;
	receiveBytes(socket, (char *)&nameLength, sizeof(nameLength));
	
	char fileName[nameLength + 1];
	receiveBytes(socket, fileName, nameLength);
	fileName[nameLength] = '\0';
	
	uint64_t fileSize;
	receiveBytes(socket, (char *)&fileSize, sizeof(fileSize));
	uint64_t fileBlock = fileSize / 20;
	
	cout << "Receiving file(" << fileSize / 1024 << " KB): " << fileName << " will be stored at: ./" << fileName << endl;
	
	ofstream file(fileName);
	uint64_t bytesRed, blockRed = 0;
	char buffer[10240];
	
	while (isOpen && fileSize > 0) {
		bytesRed = socket.read(buffer, (int)min(fileSize, (uint64_t)10240));
		fileSize -= bytesRed;
		blockRed += bytesRed;
		
		if (bytesRed != 0) {
			file.write(buffer, bytesRed);
			
			if (blockRed > fileBlock) {
				cout << "Remaining size: " << fileSize / 1024 << " KB" << endl;
				blockRed = 0;
			}
		}
	}
	
	if (isOpen)
		cout << "File received" << endl;
}

void App::receiveBytes(Socket &socket, char *buffer, uint32_t count) {
	uint32_t bytesRed = 0;
	
	while (isOpen && count > 0) {
		bytesRed = socket.read(buffer, count);
		count -= bytesRed;
		buffer += bytesRed;
	}
	
	if (!isOpen)
		throw AppNotOpenError();
}

void App::writeLoop(Socket &socket) {
	string choice;
	
	try {
		while (isOpen) {
			printMenu();
			getline(cin, choice);
			if (choice.length() != 1)
				continue;
			
			switch (choice[0]) {
				case '1':
					sendTextMessage(socket);
					break;
					
				case '2':
					sendFileMessage(socket);
					break;
					
				case '0':
					isOpen = false;
					return;
			}
		}
	} catch (AppNotOpenError e) {}
}

void App::printMenu() {
	cout << "### Komunikator Menu ###" << endl
		<< "[1] Send Message" << endl
		<< "[2] Send File" << endl
		<< "[0] Exit" << endl
		<< "Your choice: ";
	cout.flush();
}

void App::sendTextMessage(Socket &socket) {
	cout << "Enter your message: ";
	cout.flush();
	string message;
	
	readUserInput(message);
	
	writeTextMessage(socket, Message::Text {
		.length = (uint8_t)message.length(),
		.message = message,
	});
}

void App::writeTextMessage(Socket &socket, const Message::Text &message) {
	size_t messageSize = message.size();
	char buffer[messageSize];
	message.copy(buffer);
	
	socket.write(buffer, messageSize);
}

void App::sendFileMessage(Socket &socket) {
	cout << "Enter filename: ";
	cout.flush();
	string filePath;
	
	readUserInput(filePath);
	string fileName = getFileName(filePath);
	
	ifstream file(filePath);
	if (!file.is_open()) {
		cerr << "[ERR] Could not open file " << filePath << endl;
		cerr << "Error: " << strerror(errno);
		return;
	}
	
	uint64_t fileSize = file.tellg();
	file.seekg(0, ios::end);
	fileSize = (uint64_t)file.tellg() - fileSize;
	file.seekg(0);
	
	if (fileSize == 0)
		return;
	
	writeFileMessage(socket, Message::File {
		.nameLength = (uint8_t)fileName.length(),
		.fileName = fileName,
		.fileSize = fileSize,
	}, file);
}

string App::getFileName(string &path) {
	size_t index = path.rfind('/');
	
	if (index == string::npos)
		return path;
	
	return path.substr(index + 1, path.size() - index - 1);
}

void App::writeFileMessage(Socket &socket, const Message::File &message, ifstream &file) {
	size_t messageSize = message.size();
	size_t bytesRed;
	char buffer[10240];
	message.copy(buffer);
	
	socket.write(buffer, messageSize);
	
	while (true) {
		file.read(buffer, 10240);
		bytesRed = file.gcount();
		
		if (bytesRed == 0)
			break;
		
		socket.write(buffer, bytesRed);
	};
}

void App::readUserInput(string &buffer) {
	pollfd pollFD {
		.fd = STDIN_FILENO,
		.events = POLLIN,
	};
	
	while (isOpen) {
		switch (poll(&pollFD, 1, 500)) {
			case 0:
				continue;
				
			default:
				getline(cin, buffer);
				if (buffer.length() == 0)
					continue;
				
				return;
		}
	}
	
	throw AppNotOpenError();
}
