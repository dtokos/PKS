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
	
	map<int, string> parse(const string &fileName) {
		ifstream file(fileName);
		if (!file.is_open())
			throw ParsingError("File " + fileName + " could not be opened");
		
		return parse(file);
	}
	
	map<int, string> parse(istream &data) {
		map<int, string> config;
		
		string line;
		while (getline(data, line)) {
			stringstream lineStream(line);
			int number;
			string name;
			
			if (!(lineStream >> number >> name))
				continue;
			
			config[number] = name;
		}
		
		return config;
	}
};

#endif
