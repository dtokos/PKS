#ifndef PrintFramesCommand_hpp
#define PrintFramesCommand_hpp

#include "./Command.hpp"
#include "./Print.hpp"

class PrintFramesCommand : public Command {
public:
	~PrintFramesCommand() {}
	
	void process(Frame *frame) {
		Command::process(frame);
		Print::frame(frame);
	}
	
	void endProcessing() {}	
};

#endif
