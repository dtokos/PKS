#ifndef App_hpp
#define App_hpp

#include "../Parsers/PcapParser.hpp"
#include "../Commands/Command.hpp"

class App {
public:
	App(PcapParser parser, vector<Command *> commands);
	~App();
	void run(const string &fileName);
	
private:
	PcapParser parser;
	vector<Command *> commands;
	
	void processFrames(vector<Frame *> &frames);
};

#endif
