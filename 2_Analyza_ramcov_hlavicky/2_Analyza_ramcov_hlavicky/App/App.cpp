#include "App.hpp"

App::App(PcapParser parser, vector<Command *> commands) : parser(parser), commands(commands) {}
App::~App() {
	for (Command *command : commands)
		delete command;
}

void App::run(const string &fileName) {
	vector<Frame *> frames = parser.parse(fileName);
	processFrames(frames);
	
	for (Frame *frame : frames)
		delete frame;
}

void App::processFrames(vector<Frame *> &frames) {
	for (Frame *frame : frames)
		for (Command *command : commands)
			command->process(frame);
	
	for (Command *command : commands)
		command->endProcessing();
}
