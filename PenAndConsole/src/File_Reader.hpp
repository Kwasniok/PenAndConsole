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

//! @brief auxiliary universal input buffer <p>
//! Blocks represent a part or the whole content of a file as a tree-shaped graph.
//! @warning syntactic format checks only!
struct Block {
	//! type (class)
	std::string type;
	//! name of attribute or varaible (in an asignment)
	std::string name;
	//! value (of assignment)
	std::string value;
	//! nested blocks inside this block (e.g. items when list)
	std::vector<Block> sub_blocks;

	bool empty() const {
		return type.empty() && name.empty() && value.empty() && sub_blocks.empty();
	}
};

//! @waring no change when format is voilated!
bool operator>>(Block& b, Item& i);
//! @waring no change when format is voilated!
bool operator>>(Block& b, Reaction& r);
//! @waring no change when format is voilated!
bool operator>>(Block& b, Action& a);
//! @waring no change when format is voilated!
bool operator>>(Block& b, Inventory& i);
//! @waring no change when format is voilated!
bool operator>>(Block& b, Context& c);

class File_Reader : public std::ifstream {
	bool layer_forbids_enum = false;
public:
	File_Reader(const char* path) : std::ifstream(path) { }

	//! @waring no change when format is voilated!
	File_Reader& operator>>(std::vector<Block>& b);
	//! @waring no change when format is voilated!
	File_Reader& operator>>(Context& r);
	
};

#endif /* File_Reader_hpp */
