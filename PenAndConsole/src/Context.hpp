//
//  Context.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef Context_hpp
#define Context_hpp

#include <string>
#include <vector>
#include <map>

#include "Action.hpp"
#include "Inventory.hpp"

class Context {
public:
	std::vector<Action> actions;
	Inventory inventory;
	std::map<std::string, std::string> context_vars;

	Reaction get_reaction(const std::string& action);
	void evaluate_reaction(const Reaction& reaction);
	bool is_possible_action(const Action& a);
	
};

#endif /* Context_hpp */
