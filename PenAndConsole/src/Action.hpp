//
//  Action.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef Action_hpp
#define Action_hpp

#include <string>
#include <vector>

#include "Reaction.hpp"

struct Action {
	
	std::string key;
	std::vector<std::pair<std::string, std::string>> needs_context_vars_to_be;
	std::vector<Item> needs_items;
	std::vector<Item> forbids_items;
	Reaction reaction;
	bool hidden = false;
	bool trigger_once_only = false;
	bool was_triggered = false;

	bool none() const {
		return key.empty();
	}
};

#endif /* Action_hpp */
