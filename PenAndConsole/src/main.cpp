//
//  main.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 23.12.15.
//  Copyright (c) 2015 Jens Kwasniok. All rights reserved.
//

#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

template <class C, class K>
inline bool has(const C& container, const K& key) {
	return std::find(container.begin(), container.end(), key) != container.end();
}

class Console {

private:
	std::istream& cis;
	std::ostream& cos;
	std::string buffer;

	const std::string promt_in = ">> ";
	const std::string promt_out = "<< ";

public:
	Console(std::istream& cis, std::ostream& cos) : cis(cis), cos(cos) { }

	void aquire_input() {
		cos << promt_in;
		getline(cis, buffer);
	}

	std::string last_input() {
		return buffer;
	}

	void print(const std::string& s) {
		cos << promt_out << s << endl;
	}

};

struct Item {
	std::string id;

	bool operator==(const Item& rhs) {
		return this->id == rhs.id;
	}
	bool operator!=(const Item& rhs) {
		return !(*this == rhs);
	}
};

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
const Reaction Reaction::none {""};

struct Action {
	std::string key;
	std::vector<std::pair<std::string, std::string>> needs_context_vars_to_be;
	std::vector<Item> needs_items;
	Reaction reaction;
};

struct Inventory {
	std::vector<Item> items;


	void give(const Item& i) {
		items.push_back(i);
	}

	bool remove(const Item& i) {

		auto it =  std::find(items.begin(), items.end(), i);
		if (it != items.end()) {
			items.erase(it);
			return true;
		}
		else
			return false;
	}

	bool has_item(const Item& i) {
		return std::find(items.begin(), items.end(), i) != items.end();
	}

};

class Context {
public:
	std::vector<Action> actions;
	Inventory& inventory;
	std::map<std::string, std::string> context_vars;

	Reaction get_reaction(const std::string& action) {

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
				else
					return Reaction::none;
			}
		}
		return Reaction::none;
	}

	void evaluate_reaction(const Reaction& reaction) {

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

};


int main(int argc, const char * argv[]) {

	Console c {cin, cout};
	Inventory i;
	Context cxt {{}, i};
	cxt.context_vars["door"] = "closed";

	cxt.actions.push_back({"hello", {}, {}, {"no respond"}});
	cxt.actions.push_back({"open door",  {{"door", "closed"}}, {}, {"crunching noise", {{"door", "opend" }}, {}}});
	cxt.actions.push_back({"close door", {{"door", "opend" }}, {}, {"crunching noise", {{"door", "closed"}}, {}}});
	cxt.actions.push_back({"grab stone", {}, {}, {"grabbed stone", {}, {{"stone"}}, {}}});
	cxt.actions.push_back({"throw stone", {}, {{"stone"}}, {"off it goes", {}, {}, {{"stone"}}}});


	Reaction r;
	do {
		c.aquire_input();
		Reaction r = cxt.get_reaction(c.last_input());
		cxt.evaluate_reaction(r);
		if (r != Reaction::none) {
			c.print(r.description);
		}
	} while (c.last_input() != "stop");

}
