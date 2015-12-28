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
	Reaction reaction;
	bool hidden = false;
};

#endif /* Action_hpp */
