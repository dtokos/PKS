#ifndef ConfigParser_hpp
#define ConfigParser_hpp

#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

class ConfigParser {
public:
	class ParsingError : public exception {
	public:
		string message;
		
		ParsingError(string message) : message(message) {}
		const char * what () const throw () {
			return message.c_str();
		}
	};
	
	map<int, string> parse(const string &fileName);
	map<int, string> parse(istream &data);
};

#endif
