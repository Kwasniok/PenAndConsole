//
//  main.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 23.12.15.
//  Copyright (c) 2015 Jens Kwasniok. All rights reserved.
//

#include <string>
#include <iostream>
#include <fstream>
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

	bool empty() {
		return items.empty();
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

class File_Writer : public std::ofstream {
public:
	int intendation_level = 0;
	bool _wrap = false;

	File_Writer(const char* path) : ofstream(path) { }

	File_Writer& operator<<(File_Writer& (*pf) (File_Writer&)) {
		return pf(*this);
	}

	File_Writer& operator<<(ofstream& (*pf) (ofstream&)) {
		pf(*this);
		return *this;
	}

	template <class K, class V>
	File_Writer& operator<<(const pair<K, V>& p);
	template <class K, class V>
	File_Writer& operator<<(const map<K, V>& m);
	template <class T>
	File_Writer& operator<<(const vector<T>& v);
	File_Writer& operator<<(const Item& i);
	File_Writer& operator<<(const Reaction& r);
	File_Writer& operator<<(const Action& r);
	File_Writer& operator<<(const Inventory& r);
	File_Writer& operator<<(const Context& r);
};


File_Writer& nl (File_Writer& fw) {
	endl(fw);
	for (int i=0; i < fw.intendation_level; ++i)
		fw << '\t';
	return fw;
}

File_Writer& intend (File_Writer& fw) {
	++fw.intendation_level;
	fw << nl;
	return fw;
}

File_Writer& unintend (File_Writer& fw) {
	--fw.intendation_level;
	fw << nl;
	return fw;
}

File_Writer& sep (File_Writer& fw) {
	fw << ';';
	if(fw._wrap)
		fw << nl;
	return fw;
}

File_Writer& wrap (File_Writer& fw) {
	fw._wrap = true;
	return fw;
}


File_Writer& unwrap (File_Writer& fw) {
	fw._wrap = false;
	return fw;
}


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

	//
	Action ac = {"do this", {{"a", "off"}}, {{"air"}}, {"hi there!", {{"a", "on"}}, {{"bla"}}, {{"blub"}}}};
	Inventory i2;
	Context cxt2 {{}, i2};
	i2.items.push_back({"item1"});
	i2.items.push_back({"item2"});
	i2.items.push_back({"item3"});
	cxt2.actions.push_back({"action1"});
	File_Writer fw {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/save.txt"};
	fw << cxt;
	//

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


template <class K, class V>
File_Writer& File_Writer::operator<<(const pair<K, V>& p) {
	*this << '{' << p.first << ':' << p.second << '}';
	return *this;
}

template <class K, class V>
File_Writer& File_Writer::operator<<(const map<K, V>& m) {
	*this << '{';
	if (_wrap)
		*this << intend;
	bool first = true;
	for (auto it = m.begin(); it != m.end(); ++it) {
		if (!first) {
			*this << sep;
		}
		else
			first = false;
		*this << *it;
	}
	if (_wrap)
		*this << unintend;
	*this << '}';
	return *this;
}

template <class T>
File_Writer& File_Writer::operator<<(const vector<T>& v) {
	*this << '{';
	if (_wrap)
		*this << intend;
	bool first = true;
	for (auto it = v.begin(); it != v.end(); ++it) {
		if (!first) {
			*this << sep;
		}
		else
			first = false;
		*this << *it;
	}
	if (_wrap)
		*this << unintend;
	*this << '}';
	return *this;
}


File_Writer& File_Writer::operator<<(const Item& i) {
	*this << '"' << i.id << '"';
	return *this;
}


File_Writer& File_Writer::operator<<(const Reaction& r) {
	*this << "Reaction {";
	*this << intend;

	*this << "description = \"" << r.description << '"';

	if (!r.set_context_vars.empty()) {
		*this << sep;
		*this << "set_cxt_vars = ";
		*this << r.set_context_vars;
	}

	if (!r.give_items.empty()) {
		*this << sep;
		*this << "give_items = ";
		*this << r.give_items;
	}

	if (!r.take_items.empty()) {
		*this << sep;
		*this << "take_items = ";
		*this<< r.take_items;
	}

	*this << unintend;
 	*this << '}';
	return *this;
}

File_Writer& File_Writer::operator<<(const Action& a) {
	*this << "Action {";
	*this << intend;

	*this << "key = \"" << a.key << '"';

	if (!a.needs_context_vars_to_be.empty()) {
		*this << sep;
		*this << "needes_cxt_vars = ";
		*this << a.needs_context_vars_to_be;
	}

	if (!a.needs_items.empty()) {
		*this << sep;
		*this << "neddes_items = ";
		*this << a.needs_items;
	}

	*this << sep;
	*this << "reaction = ";
	*this << a.reaction;

	*this << unintend;
	*this << '}';
	return *this;
}

File_Writer& File_Writer::operator<<(const Inventory& i) {
	*this << "Inventory ";
	*this << i.items;
	return *this;
}

File_Writer& File_Writer::operator<<(const Context& c) {
	*this << "Context {";
	*this << intend << wrap;

	bool first = true;

	if (!c.actions.empty()){
		first = false;
		*this << "actions = ";
		*this << c.actions;
	}

	if (!c.inventory.empty()) {
		if(!first) {*this << sep; first = false;}
		*this << "inventory = ";
		*this << c.inventory;
	}

	if (!c.context_vars.empty()) {
		if(!first) {*this << sep; first = false;}
		*this << "cxt_vars = ";
		*this << c.context_vars;
	}

	*this << unwrap << unintend;
	*this << '}';
	this->flush();

	return *this;
}

