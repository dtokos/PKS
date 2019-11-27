#ifndef PrintFramesCommand_hpp
#define PrintFramesCommand_hpp

#include "./Command.hpp"

class PrintFramesCommand : public Command {
public:
	~PrintFramesCommand() {}
	
	void process(Frame *frame) {
		printFrameHeader(frame);
		printAllFrameData(frame);
	}
	
	void endProcessing() {}
	
private:
	void printFrameHeader(Frame *frame) {
		cout << dec
			<< "rámec " << frame->serialNumber << endl
			<< "dĺžka rámca poskytnutá pcap API –  " << frame->pcapLength << " B" << endl
			<< "dĺžka rámca prenášaného po médiu – " << frame->wireLength() << " B" << endl
			<< frame->name() << endl
			<< "Zdrojová MAC adresa: " << frame->source().asString(' ') << endl
			<< "Cieľová MAC adresa:  " << frame->destination().asString(' ') << endl;
		
		if (frame->packet != NULL)
			cout << frame->packet->name() << endl;
	}
	
	void printAllFrameData(Frame *frame) {
		uint8_t *byte = frame->raw;
		int i;
		
		for (i = 0; i < frame->capturedLength; i++, byte++) {
			cout << setfill('0') << setw(2) << hex << static_cast<int>(*byte) << " ";
			
			if (i == 0)
				continue;
			if (i % 16 == 15)
				cout << endl;
			else if (i % 8 == 7)
				cout << "\t";
		}
		
		cout << endl;
		
		if (i % 16 != 0)
			cout << endl;
	}
};

#endif
