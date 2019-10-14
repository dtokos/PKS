#ifndef IP_hpp
#define IP_hpp

#include <string>
#include <exception>
#include <arpa/inet.h>

using namespace std;

class IP {
public:
	class InvalidIP : public exception {
	public:
		string ip;
		
		InvalidIP(string ip) : ip(ip) {}
		
		const char * what () const throw () {
			return "Given ip is not valid";
		}
	};
	
	static IP fromString(string addressString);
	
private:
	IP(in_addr address);
	in_addr address;
};

#endif
