#ifndef CommunicationCommand_hpp
#define CommunicationCommand_hpp

#include "./Command.hpp"
#include "./Communications/Communications.hpp"
#include "./Print.hpp"

class CommunicationCommand : public Command {
public:
	CommunicationCommand(int lengthLimit) : lengthLimit(lengthLimit) {}
	~CommunicationCommand() {for (Communication * communication : communications) delete communication;}
	
	void process(Frame *frame) {
		Command::process(frame);
		
		if (!addToCommunication())
			newCommunication();
	}
	
	void endProcessing() {
		bool didPrintTCPComplete = false, didPrintTCPIncomplete = false;
		int index = 1;
		
		for (Communication *communication : communications) {
			if (shouldPrint(communication, didPrintTCPComplete, didPrintTCPIncomplete)) {
				cout << "Komunikacia č. " << dec << index++ << endl;
				communication->print(lengthLimit);
			}
		}
	}
	
private:
	int lengthLimit;
	vector<Communication *>communications;
	
	bool addToCommunication() {
		for (Communication *communication : communications)
			if (communication->add(frame))
				return true;
		
		return false;
	}
	
	void newCommunication() {
		if (segment != NULL) {
			switch (segment->segmentType()) {
				case Segment::TCP:
					communications.push_back((Communication *)new TCPCommunication(frame));
					break;
				
				case Segment::UDP:
					communications.push_back(
						segment->message != NULL && segment->message->messageType() == Message::TFTP ?
							(Communication *)new TFTPCommunication(frame) :
							(Communication *)new UDPCommunication(frame)
					);
					break;
					
				case Segment::ICMP:
					communications.push_back((Communication *)new ICMPCommunication(frame));
					break;
					
				default:
					break;
			}
		}
	}
	
	bool shouldPrint(Communication *communication, bool &complete, bool &incomplete) {
		if (communication->usesConnection() && ((!complete && communication->isComplete()) || (!incomplete && !communication->isComplete()))) {
			complete |= communication->isComplete();
			incomplete |= !communication->isComplete();
			
			return true;
		} else if (!communication->usesConnection())
			return true;
		
		return false;
	}
};

#endif
