#ifndef TopIPsCommand_hpp
#define TopIPsCommand_hpp

#include "./Command.hpp"

class TopIPsCommand : public Command {
public:
	TopIPsCommand() {}
	
	void process(Frame *frame) {
		if (frame->frameType() != Frame::EthernetII || frame->packet == NULL || frame->packet->packetType() != Packet::IPv4)
			return;
		
		updateCount((IPv4Packet *)frame->packet);
	}
	
	void endProcessing() {
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
		
		if (counts.size() == 1 || counts.count(source) > counts.count(topIP))
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
			cout << "Adresa uzla s najväčším počtom odoslaných paketov:" << endl
				<< topIP << "\t" << counts.at(topIP) << " paketov" << endl;
	}
};

#endif
