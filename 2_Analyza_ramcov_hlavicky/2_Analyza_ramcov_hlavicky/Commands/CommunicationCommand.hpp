#ifndef CommunicationCommand_hpp
#define CommunicationCommand_hpp

#include "./Command.hpp"

using Flags = TCPSegment::Flags;

class CommunicationCommand : public Command {
public:
	CommunicationCommand(int lengthLimit) : lengthLimit(lengthLimit) {}
	
	void process(Frame *frame) {
		Command::process(frame);
		
		if (segment && segment->segmentType() == Segment::Type::TCP)
			addToCommunication((TCPSegment *)segment);
	}
	
	void endProcessing() {
		cout << "Comm " << (int)comm->isComplete() << endl;
	}
	
private:
	class Communication {
	public:
		Communication(TCPSegment *segment) : segments({segment}) {};
		
		void add(TCPSegment *segment) {
			segments.push_back(segment);
			
			switch (currentState) {
				case Opened:
					cout << "Current State Opened" << endl;
					handleOpened(segment);
					break;
					
				case SenderFIN:
					cout << "Current State SenderFIN" << endl;
					handleSenderFIN(segment);
					break;
					
				case ReceiverACK:
					cout << "Current State ReceiverACK" << endl;
					handleReceiverACK(segment);
					break;
					
				case ReceiverFIN:
					cout << "Current State ReceiverFIN" << endl;
					handleReceiverFIN(segment);
					break;
					
				case Closed:
					cout << "Current State Closed" << endl;
					break;
			}
		}
		
		bool isComplete() {return currentState == Closed;}
	private:
		enum State {Opened, SenderFIN, ReceiverACK, ReceiverFIN, Closed};
		vector<TCPSegment *> segments;
		State currentState = Opened;
		
		void handleOpened(TCPSegment *segment) {
			if (segment->flags().is(Flags::RST)) {
				cout << "Received RST Next State Closed" << endl;
				currentState = Closed;
			} else if (segment->flags().is(Flags::FIN)) {
				cout << "Received FIN Next State SenderFIN" << endl;
				currentState = SenderFIN;
			}
		}
		
		void handleSenderFIN(TCPSegment *segment) {
			if (segment->flags().is(Flags::RST)) {
				cout << "Received RST Next State Closed" << endl;
				currentState = Closed;
			} else if (segment->flags().is(Flags::FIN) && segment->flags().is(Flags::ACK)) {
				cout << "Received FIN_ACK Next State ReceiverFIN" << endl;
				currentState = ReceiverFIN;
			} else if (segment->flags().is(Flags::ACK)) {
				cout << "Received ACK Next State ReceiverACK" << endl;
				currentState = ReceiverACK;
			}
		}
		
		void handleReceiverACK(TCPSegment *segment) {
			if (segment->flags().is(Flags::RST)) {
				cout << "Received RST Next State Closed" << endl;
				currentState = Closed;
			} else if (segment->flags().is(Flags::FIN)) {
				cout << "Received FIN Next State ReceiverFIN" << endl;
				currentState = ReceiverFIN;
			}
		}
		
		void handleReceiverFIN(TCPSegment *segment) {
			if (segment->flags().is(Flags::RST) || segment->flags().is(Flags::ACK)) {
				cout << "Received RST | ACK Next State Closed" << endl;
				currentState = Closed;
			}
		}
	};
	
	int lengthLimit;
	Communication *comm = NULL;
	
	void addToCommunication(TCPSegment *segment) {
		if (comm == NULL)
			comm = new Communication(segment);
		else
			comm->add(segment);
	}
};

#endif
