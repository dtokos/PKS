#ifndef App_hpp
#define App_hpp

#include "../Network/PcapParser.hpp"

class App {
public:
	App(PcapParser parser);
	void run(const string &fileName);
	
private:
	PcapParser parser;
	
	void printAllFrames(vector<Frame *> &frames);
	void printFrameHeader(Frame *frame);
	void printAllFrameData(Frame *frame);
};

#endif
