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

	//! string user has to type in <p>
	//! convention: leading @ for actions triggered internaly
	std::string key;
	std::vector<std::pair<std::string, std::string>> needs_context_vars_to_be;
	//! player MUST have these items in inventory
	std::vector<Item> needs_items;
	//! player MUST NOT have these items in inventory
	std::vector<Item> forbids_items;
	Reaction reaction;
	//! action should not be displayed as possible even when <p>
	//! used for supprises and internal actions etc.
	bool hidden = false;
	bool trigger_once_only = false;
	bool was_triggered = false;

	//! @return true if action is meaningless and therefore should not be executed
	bool none() const {
		return key.empty();
	}
};

#endif /* Action_hpp */
