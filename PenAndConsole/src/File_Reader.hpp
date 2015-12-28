//
//  File_Reader.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef File_Reader_hpp
#define File_Reader_hpp

#include <string>
#include <fstream>
#include <vector>

#include "Context.hpp"

// auxiliary universal input buffer
struct Block {
	std::string type;
	std::string name;
	std::string value;
	std::vector<Block> sub_blocks;

	bool empty() const {
		return type.empty() && name.empty() && value.empty() && sub_blocks.empty();
	}
};

bool operator>>(Block& b, Item& i);
bool operator>>(Block& b, Reaction& r);
bool operator>>(Block& b, Action& a);
bool operator>>(Block& b, Inventory& i);
bool operator>>(Block& b, Context& c);

class File_Reader : public std::ifstream {
	bool layer_forbids_enum = false;
public:
	File_Reader(const char* path) : std::ifstream(path) { }

	std::string next_word_until(bool (*is_func) (const char c), bool putback);
	File_Reader& operator>>(std::vector<Block>& b);

	File_Reader& operator>>(Context& r);
	
};

#endif /* File_Reader_hpp */
