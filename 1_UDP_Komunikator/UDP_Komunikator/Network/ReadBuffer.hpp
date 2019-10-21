#ifndef ReadBuffer_hpp
#define ReadBuffer_hpp

#include "Segment.hpp"

class ReadBuffer {
public:
	static const uint16_t BufferSize = Segment::MaxDataLength * 8;
	bool canFitWholeSegment();
	bool hasData();
	void add(Segment &segment);
	uint16_t read(char *buffer, uint16_t length);
	
private:
	char buffer[BufferSize] = {0};
	uint16_t readIndex = 0, unreadCount = 0;
	
	uint16_t availableSize();
	char *getWritePointer();
};

#endif
