//
//  Context.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#include "Context.hpp"

Reaction Context::get_reaction(const std::string& action) {

	for (auto ac = actions.begin(); ac != actions.end(); ++ac) {
		if (action == ac->key) {

			bool context_vars_okay = true;
			for (auto cv = ac->needs_context_vars_to_be.begin(); cv != ac->needs_context_vars_to_be.end(); ++cv) {
				if (context_vars[cv->first] != cv->second) {
					context_vars_okay = false;
					break;
				}
			}

			bool has_items = true;
			for (auto ni = ac->needs_items.begin(); ni != ac->needs_items.end(); ++ni) {
				if (!inventory.has_item(*ni)) {
					has_items = false;
					break;
				}
			}

			if (context_vars_okay & has_items)
				return ac->reaction;
		}
	}
	return Reaction::none;
}

void Context::evaluate_reaction(const Reaction& reaction) {

	for (auto i = reaction.give_items.begin(); i != reaction.give_items.end(); ++i) {
		inventory.give(*i);
	}

	for (auto i = reaction.take_items.begin(); i != reaction.take_items.end(); ++i) {
		inventory.remove(*i);
	}

	for (auto cv = reaction.set_context_vars.begin(); cv != reaction.set_context_vars.end(); ++cv) {
		context_vars[cv->first] = cv->second;
	}
}
