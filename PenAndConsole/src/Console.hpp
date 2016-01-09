//
//  Console.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef Console_hpp
#define Console_hpp

#include <iostream>

class Console {

private:
	std::istream& cis;
	std::ostream& cos;
	std::string buffer; // contains last line of input

	 // added before each input/output line
	const std::string promt_in = ">> ";
	const std::string promt_out = "";

public:
	//! cis & cos should refer to the cosole IO
	Console(std::istream& cis, std::ostream& cos) : cis(cis), cos(cos) { }

	//! get next input line
	void aquire_input() {
		cos << promt_in;
		getline(cis, buffer, '\n');
	}

	//! change last line of input silently
	void ghost_write(const std::string& ghost_input) {
		buffer = ghost_input;
	}

	std::string last_input() {
		return buffer;
	}

	void print(const std::string& s) {
		cos << promt_out << s << std::endl;
	}

	std::ostream& costream() {return cos;}
	
};

#endif /* Console_hpp */
