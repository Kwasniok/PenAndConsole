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
	bool wrap = false;

	File_Writer(const char* path) : ofstream(path) { }

	File_Writer& operator<<(File_Writer& (*pf) (File_Writer&)) {
		return pf(*this);
	}

	File_Writer& operator<<(ofstream& (*pf) (ofstream&)) {
		pf(*this);
		return *this;
	}
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
	if(fw.wrap)
		fw << nl;
	return fw;
}

File_Writer& wrap (File_Writer& fw) {
	fw.wrap = true;
	return fw;
}



File_Writer& unwrap (File_Writer& fw) {
	fw.wrap = false;
	return fw;
}


template <class K, class V>
void write_to_stream(File_Writer& os, const pair<K, V>& p);

template <class K, class V>
void write_to_stream(File_Writer& os, const map<K, V>& m);

template <class T>
void write_to_stream(File_Writer& os, const vector<T>& v);

void write_to_stream(File_Writer& os, const Item& i);

void write_to_stream(File_Writer& os, const Reaction& r);

void write_to_stream(File_Writer& os, const Action& r);

void write_to_stream(File_Writer& os, const Inventory& r);

void write_to_stream(File_Writer& os, const Context& r);


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
	write_to_stream(fw, cxt);
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
void write_to_stream(File_Writer& os, const pair<K, V>& p) {
	os << '{' << p.first << ':' << p.second << '}';
}

template <class K, class V>
void write_to_stream(File_Writer& os, const map<K, V>& m) {
	os << '{';
	if (os.wrap)
		os << intend;
	bool first = true;
	for (auto it = m.begin(); it != m.end(); ++it) {
		if (!first) {
			os << sep;
		}
		else
			first = false;
		write_to_stream(os, *it);
	}
	if (os.wrap)
		os << unintend;
	os << '}';
}

template <class T>
void write_to_stream(File_Writer& os, const vector<T>& v) {
	os << '{';
	if (os.wrap)
		os << intend;
	bool first = true;
	for (auto it = v.begin(); it != v.end(); ++it) {
		if (!first) {
			os << sep;
		}
		else
			first = false;
		write_to_stream(os, *it);
	}
	if (os.wrap)
		os << unintend;
	os << '}';
}


void write_to_stream(File_Writer& os, const Item& i) {
		os << '"' << i.id << '"';
}


void write_to_stream(File_Writer& os, const Reaction& r) {
	os << "Reaction {";
	os << intend;

	os << "description = \"" << r.description << '"';

	if (!r.set_context_vars.empty()) {
		os << sep;
		os << "set_cxt_vars = ";
		write_to_stream(os, r.set_context_vars);
	}

	if (!r.give_items.empty()) {
		os << sep;
		os << "give_items = ";
		write_to_stream(os, r.give_items);
	}

	if (!r.take_items.empty()) {
		os << sep;
		os << "take_items = ";
		write_to_stream(os, r.take_items);
	}

	os << unintend;
	os << '}';
}

void write_to_stream(File_Writer& os, const Action& a) {
	os << "Action {";
	os << intend;

	os << "key = \"" << a.key << '"';

	if (!a.needs_context_vars_to_be.empty()) {
		os << sep;
		os << "needes_cxt_vars = ";
		write_to_stream(os, a.needs_context_vars_to_be);
	}

	if (!a.needs_items.empty()) {
		os << sep;
		os << "neddes_items = ";
		write_to_stream(os, a.needs_items);
	}

	os << sep;
	os << "reaction = ";
	write_to_stream(os, a.reaction);

	os << unintend;
	os << '}';
}

void write_to_stream(File_Writer& os, const Inventory& i) {
	os << "Inventory ";
	//os << intend;
	write_to_stream(os, i.items);
	//os << unintend;
}

void write_to_stream(File_Writer& os, const Context& c) {
	os << "Context {";
	os << intend;
	os << wrap;

	bool first = true;

	if (!c.actions.empty()){
		first = false;
		os << "actions = ";
		write_to_stream(os, c.actions);
	}

	if (!c.inventory.empty()) {
		if(!first) {os << sep; first = false;}
		os << "inventory = ";
		write_to_stream(os, c.inventory);
	}

	if (!c.context_vars.empty()) {
		if(!first) {os << sep; first = false;}
		os << "cxt_vars = ";
		write_to_stream(os, c.context_vars);
	}

	os << unwrap;
	os << unintend;
 	os << '}';
	os.flush();
}

