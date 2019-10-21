#include "ReadBuffer.hpp"

bool ReadBuffer::canFitWholeSegment() {
	return availableSize() >= Segment::MaxDataLength;
}

bool ReadBuffer::hasData() {
	return unreadCount > 0;
}

uint16_t ReadBuffer::availableSize() {
	return BufferSize - unreadCount;
}

void ReadBuffer::add(Segment &segment) {
	uint16_t bytesAtEnd = BufferSize - readIndex - unreadCount;
	uint16_t bytesToAdd = segment.dataLength();
	uint16_t bytesAdded = min(bytesAtEnd, bytesToAdd);
	char *data = segment.getData();
	
	memcpy(getWritePointer(), data, bytesAdded);
	unreadCount += bytesAdded;
	bytesToAdd -= bytesAdded;
	data += bytesAdded;
	
	if (bytesToAdd > 0) {
		memcpy(buffer, data, bytesToAdd);
		unreadCount += bytesToAdd;
	}
}

char* ReadBuffer::getWritePointer() {
	return buffer + ((readIndex + unreadCount) % BufferSize);
}

uint16_t ReadBuffer::read(char *destination, uint16_t length) {
	uint16_t bytesToRead = min(length, unreadCount);
	uint16_t bytesUntilEnd = BufferSize - readIndex;
	bytesUntilEnd = min(bytesUntilEnd, unreadCount);
	uint16_t bytesRed = 0;
	
	if (bytesUntilEnd && bytesToRead) {
		uint16_t bytesRedAtEnd = min(bytesToRead, bytesUntilEnd);
		bytesRed += bytesRedAtEnd;
		bytesToRead -= bytesRedAtEnd;
		
		memcpy(destination, buffer + readIndex, bytesRedAtEnd);
		destination += bytesRedAtEnd;
		
		unreadCount -= bytesRedAtEnd;
		readIndex = (readIndex + bytesRedAtEnd) % BufferSize;
	}
	
	if (bytesToRead > 0 && unreadCount > 0) {
		uint16_t bytesRedAtStart = min(bytesToRead, unreadCount);
		bytesRed += bytesRedAtStart;
		memcpy(destination, buffer, bytesRedAtStart);
		unreadCount -= bytesRedAtStart;
		readIndex = (readIndex + bytesRedAtStart) % BufferSize;
	}
	
	return bytesRed;
}
