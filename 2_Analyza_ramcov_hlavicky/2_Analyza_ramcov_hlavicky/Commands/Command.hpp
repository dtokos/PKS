#ifndef Command_hpp
#define Command_hpp

#include "../Network/Frames/Frame.hpp"

class Command {
public:
	virtual ~Command() {};
	
	virtual void process(Frame *frame) {
		this->frame = frame;
		this->packet = frame->packet;
		this->segment = packet == NULL ? NULL : packet->segment;
		this->message = segment == NULL ? NULL : segment->message;
	};
	virtual void endProcessing() = 0;
	
protected:
	Frame *frame;
	Packet *packet;
	Segment *segment;
	Message *message;
};

#endif
