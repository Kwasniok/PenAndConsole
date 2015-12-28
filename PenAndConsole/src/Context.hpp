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
	static Action none_action;

	std::vector<Action> actions;
	Inventory inventory;
	std::map<std::string, std::string> context_vars;

	Action& get_action(const std::string& action);
	void evaluate_action(Action& action);
	bool is_possible_action(const Action& a);
	
};

#endif /* Context_hpp */
