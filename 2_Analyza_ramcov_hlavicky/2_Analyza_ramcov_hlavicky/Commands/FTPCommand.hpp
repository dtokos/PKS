#ifndef FTPCommand_hpp
#define FTPCommand_hpp

#include "./Command.hpp"

class FTPCommand : public Command {
public:
	void process(Frame *frame) {
		Command::process(frame);
		
		if (isFTP())
			frames.push_back(frame);
	}
	
	virtual void endProcessing() {
		cout << dec << "Počet FTP dátových rámcov: " << frames.size() << endl;
		for (Frame *frame : frames)
			Print::frame(frame);
	}
	
private:
	vector<Frame *>frames;
	
	bool isFTP() {
		return segment != NULL && segment->segmentType() == Segment::TCP && ((message != NULL && message->messageType() == Message::FTP) || checkDestinations());
	}
	
	bool checkDestinations() {
		for (Frame *frame : frames)
			if (((TCPSegment *)frame->segment)->destinationPort() == ((TCPSegment *)segment)->sourcePort())
				return true;
		return false;
	}
};

#endif
