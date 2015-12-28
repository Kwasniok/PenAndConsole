//
//  Reaction.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef Reaction_hpp
#define Reaction_hpp

#include <string>
#include <vector>

#include "Item.hpp"

struct Reaction {
	static const Reaction none;

	std::string description;
	std::vector<std::pair<std::string, std::string>> set_context_vars;
	std::vector<Item> give_items;
	std::vector<Item> take_items;

	bool operator==(const Reaction& rhs) {
		return this->description == rhs.description;
	}
	bool operator!=(const Reaction& rhs) {
		return !(*this == rhs);
	}
};

#endif /* Reaction_hpp */
