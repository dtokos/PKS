#ifndef TopIPsCommand_hpp
#define TopIPsCommand_hpp

#include "./Command.hpp"

class TopIPsCommand : public Command {
public:
	TopIPsCommand() {}
	
	void process(Frame *frame) {
		Command::process(frame);
		if (packet && packet->packetType() == Packet::IPv4)
			updateCount((IPv4Packet *)frame->packet);
	}
	
	void endProcessing() {
		if (counts.size() == 0)
			return;
		
		printAllIPs();
		printTopIP();
	}
	
private:
	map<string, int> counts;
	string topIP;
	
	void updateCount(IPv4Packet *packet) {
		string source = packet->sourceAddress().asString();
		
		if (counts.count(source))
			counts[source]++;
		else
			counts[source] = 1;
		
		if (counts.size() == 1 || (counts.count(source) && counts.at(source) > counts.at(topIP)))
			topIP = source;
	}
	
	void printAllIPs() {
		cout << "IP adresy vysielajúcich uzlov:" << endl;
		
		for (const auto &pair : counts)
			cout << pair.first << endl;
		
		cout << endl;
	}
	
	void printTopIP() {
		if (counts.size() > 0)
			cout << dec << "Adresa uzla s najväčším počtom odoslaných paketov:" << endl
				<< topIP << "\t" << (int)counts.at(topIP) << " paketov" << endl;
	}
};

#endif
