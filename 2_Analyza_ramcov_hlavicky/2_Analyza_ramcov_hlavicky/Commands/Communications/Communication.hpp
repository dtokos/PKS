#ifndef Communication_hpp
#define Communication_hpp

class Communication {
public:
	virtual ~Communication() {}
	virtual bool add(Frame *frame) = 0;
	virtual bool isComplete() = 0;
	virtual bool usesConnection() = 0;
	virtual void print(int length) = 0;
};

#endif
