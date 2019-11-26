#ifndef Command_hpp
#define Command_hpp

#include "../Network/Frames/Frame.hpp"

class Command {
public:
	virtual ~Command() {};
	
	virtual void process(Frame *frame) = 0;
	virtual void endProcessing() = 0;
};

#endif
