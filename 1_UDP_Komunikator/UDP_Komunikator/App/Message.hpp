#ifndef Message_hpp
#define Message_hpp

#include <iostream>

using namespace std;

namespace Message {
	enum Type : uint8_t {
		TextMessage = 0,
		FileMessage
	};
	
	struct Text {
		const Type type = Type::TextMessage;
		uint8_t length;
		string message;
		
		size_t size() const {
			return sizeof(Type) + sizeof(length) + message.length();
		}
		
		void copy(char *buffer) const {
			memcpy(buffer, &type, sizeof(type));
			buffer += sizeof(type);
			memcpy(buffer, &length, sizeof(length));
			buffer += sizeof(length);
			memcpy(buffer, message.c_str(), message.length());
		}
	};
	
	struct File {
		const Type type = Type::FileMessage;
		uint8_t nameLength;
		string fileName;
		uint64_t fileSize;
		
		size_t size() const {
			return sizeof(Type) + sizeof(nameLength) + nameLength + sizeof(fileSize);
		}
		
		void copy(char *buffer) const {
			memcpy(buffer, &type, sizeof(type));
			buffer += sizeof(type);
			memcpy(buffer, &nameLength, sizeof(nameLength));
			buffer += sizeof(nameLength);
			memcpy(buffer, fileName.c_str(), nameLength);
			buffer += nameLength;
			memcpy(buffer, &fileSize, sizeof(fileSize));
		}
	};
}

#endif
