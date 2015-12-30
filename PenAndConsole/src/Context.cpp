//
//  Context.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#include "Context.hpp"

Action Context::none_action {};

Action& Context::get_action(const std::string& action) {

	for (auto ac = actions.begin(); ac != actions.end(); ++ac) {
		if (action == ac->key && is_possible_action(*ac)) {
			return *ac;
		}
	}
	return none_action;
}

void Context::evaluate_action(Action& action) {

	if (action.trigger_once_only)
		action.was_triggered = true;

	for (auto i = action.reaction.give_items.begin(); i != action.reaction.give_items.end(); ++i) {
		inventory.give(*i);
	}

	for (auto i = action.reaction.take_items.begin(); i != action.reaction.take_items.end(); ++i) {
		inventory.remove(*i);
	}

	for (auto cv = action.reaction.set_context_vars.begin(); cv != action.reaction.set_context_vars.end(); ++cv) {
		context_vars[cv->first] = cv->second;
	}
}

bool Context::is_possible_action(const Action& ac) {

	if (ac.trigger_once_only && ac.was_triggered) {
		return false;
	}

	for (auto cv = ac.needs_context_vars_to_be.begin(); cv != ac.needs_context_vars_to_be.end(); ++cv) {
		if (cv->second.length() == 0) continue;

		std::string requested_value;
		bool negate = false;
		int i = 0;
		char c;
		for ( ; i<cv->second.length(); ++i) {
			c = cv->second[i];
			if (c == '!') negate = !negate;
			else break;
		}
		requested_value = cv->second.substr(i);

		if (negate) {
			if (context_vars[cv->first] == requested_value) {
				return false;
			}
		} else {
			if (context_vars[cv->first] != requested_value) {
				return false;
			}
		}
	}

	for (auto ni = ac.needs_items.begin(); ni != ac.needs_items.end(); ++ni) {
		if (!inventory.has_item(*ni)) {
			return false;;
		}
	}

	for (auto ni = ac.forbids_items.begin(); ni != ac.forbids_items.end(); ++ni) {
		if (inventory.has_item(*ni)) {
			return false;;
		}
	}

	return true;
}
