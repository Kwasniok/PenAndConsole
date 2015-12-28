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

	std::string description;
	std::vector<std::pair<std::string, std::string>> set_context_vars;
	std::vector<Item> give_items;
	std::vector<Item> take_items;

	bool has_description() {return !description.empty();}

	bool none() const {
		return description.empty()
			&& set_context_vars.empty()
			&& give_items.empty()
			&& take_items.empty();
	}
};

#endif /* Reaction_hpp */
