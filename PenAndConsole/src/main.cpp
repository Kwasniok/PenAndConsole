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

	std::ostream& costream() {return cos;}

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

	bool empty() const {
		return items.empty();
	}

	bool has_item(const Item& i) {
		return std::find(items.begin(), items.end(), i) != items.end();
	}
};

class Context {
public:
	std::vector<Action> actions;
	Inventory inventory;
	std::map<std::string, std::string> context_vars;

	Reaction get_reaction(const string& action) {

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
	bool _escape = false;

	File_Writer(const char* path) : ofstream(path) { }

	File_Writer& operator<<(File_Writer& (*pf) (File_Writer&)) {
		return pf(*this);
	}

	File_Writer& operator<<(ofstream& (*pf) (ofstream&)) {
		pf(*this);
		return *this;
	}

	File_Writer& operator<<(const char* str);
	File_Writer& operator<<(const string& str);
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

File_Writer& escape (File_Writer& fw) {
	fw._escape = true;
	return fw;
}


File_Writer& unescape (File_Writer& fw) {
	fw._escape = false;
	return fw;
}

// auxiliary universal input buffer
struct Block {
	string type;
	string name;
	string value;
	vector<Block> sub_blocks;

	bool empty() {
		return type.empty() && name.empty() && value.empty() && sub_blocks.empty();
	}
};

class File_Reader : public ifstream {
	bool layer_forbids_enum = false;
public:
	File_Reader(const char* path) : ifstream(path) { }

	string next_word_until(bool (*is_func) (const char c), bool putback);
	File_Reader& operator>>(vector<Block>& b);

	File_Reader& operator>>(Context& r);

};

bool operator>>(Block& b, Item& i);
bool operator>>(Block& b, Reaction& r);
bool operator>>(Block& b, Action& a);
bool operator>>(Block& b, Inventory& i);
bool operator>>(Block& b, Context& c);

int main(int argc, const char * argv[]) {

	// Various Tests //
	// basic setup
	Console c {cin, cout};
	Context cxt;
	 
	// Various Tests //
	/*
	// simple world
	cxt.context_vars["door"] = "closed";
	cxt.inventory.items.push_back({"stick"});
	cxt.actions.push_back({"hello", {}, {}, {"no respond"}});
	cxt.actions.push_back({"open door",  {{"door", "closed"}}, {}, {"crunching noise", {{"door", "opend" }}, {}}});
	cxt.actions.push_back({"close door", {{"door", "opend" }}, {}, {"crunching noise", {{"door", "closed"}}, {}}});
	cxt.actions.push_back({"grab stone", {}, {}, {"grabbed stone", {}, {{"stone"}}, {}}});
	cxt.actions.push_back({"throw stone", {}, {{"stone"}}, {"off it goes", {}, {}, {{"stone"}}}});
	*/

	// IO tests
	/*
	File_Writer fw1 {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/save.txt"};
	fw1 << cxt;
	Inventory i3;
	Context cxt3;
	//
	File_Reader fr4 {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/save.txt"};
	vector<Block> bs;
	fr4 >> bs;
	Context cxt4;
	bs[0] >> cxt4;
	File_Writer fw4 {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/save4.txt"};
	fw4 << cxt4;
	 */

	do {
		c.aquire_input();

		if (c.last_input() == "load game") {
			File_Reader fr {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/save.txt"};
			Context tmp;
			fr >> tmp;
			if (!fr.good()) {
				c.print("loaded game");
				cxt = std::move(tmp);
			} else {
				c.print("could not read file.");
			}
			fr.close();
		}

		if (c.last_input() == "save game") {
			File_Writer fw {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/save.txt"};
			fw << cxt;
			if (fw.good()) {
				c.print("game saved");
			} else {
				c.print("game not saved due to file error!");
			}
			fw.close();
		}

		if (c.last_input() == "show game stats") {
			c.costream() << endl
			<< "\t   -==- STATS -==- " << endl
			<< "\tactions loaded:     " << cxt.actions.size() << endl
			<< "\titmes in inventory: " << cxt.inventory.items.size() << endl
			<< "\tcontext vars:       " << cxt.context_vars.size() << endl <<endl;

		}

		if (c.last_input() == "show debug info") {
			c.costream() << endl << "inventory: ";
			bool first = true;
			for (auto it=cxt.inventory.items.begin(); it!=cxt.inventory.items.end(); ++it) {
				if (!first) c.costream() << " ~ ";
				first = false;
				c.costream() << it->id;
			}
			c.costream() << endl;
			c.costream() << endl << "actions:";
			c.costream() << endl << "\t key @{needs item | needs context var to be}"
						 << endl << "\t -> response @ {gives itmes | takes items | set context vars to}" << endl;
			first = true;
			for (auto it=cxt.actions.begin(); it!=cxt.actions.end(); ++it) {
				c.costream() << endl << '\t';
				first = false;
				c.costream() << it->key << " @{";
				bool first2 = true;
				for (auto it2=it->needs_items.begin(); it2!=it->needs_items.end(); ++it2) {
					if (!first2) c.costream() << ", ";
					first2 = false;
					c.costream() << it2->id;
				}
				c.costream() << " | ";
				first2 = true;
				for (auto it2=it->needs_context_vars_to_be.begin(); it2!=it->needs_context_vars_to_be.end(); ++it2) {
					if (!first2) c.costream() << ", ";
					first2 = false;
					c.costream() << it2->first << " : " << it2->second;
				}
				c.costream() << "}";
				c.costream() << endl << "\t -> " << it->reaction.description << " @{";
				first2 = true;
				for (auto it2=it->reaction.give_items.begin(); it2!=it->reaction.give_items.end(); ++it2) {
					if (!first2) c.costream() << ", ";
					first2 = false;
					c.costream() << it2->id;
				}
				c.costream() << " | ";
				first2 = true;
				for (auto it2=it->reaction.take_items.begin(); it2!=it->reaction.take_items.end(); ++it2) {
					if (!first2) c.costream() << ", ";
					first2 = false;
					c.costream() << it2->id;
				}
				c.costream() << " | ";
				first2 = true;
				for (auto it2=it->reaction.set_context_vars.begin(); it2!=it->reaction.set_context_vars.end(); ++it2) {
					if (!first2) c.costream() << ", ";
					first2 = false;
					c.costream() << it2->first << " : " << it2->second;
				}
				c.costream() << "}";
			}
			c.costream() << endl;
			c.costream() << endl << "context vars: ";
			first = true;
			for (auto it=cxt.context_vars.begin(); it!=cxt.context_vars.end(); ++it) {
				c.costream() << endl << '\t';
				first = false;
				c.costream() << it->first << " : " << it->second;
			}
			c.costream() << endl << endl;

		}

		if (c.last_input() == "show inventory") {
			bool first = true;
			for (auto it=cxt.inventory.items.begin(); it!=cxt.inventory.items.end(); ++it) {
				if (!first) c.costream() << " ~ ";
				first = false;
				c.costream() << it->id;
			}
			c.costream() << endl;
		}

		Reaction r = cxt.get_reaction(c.last_input());
		cxt.evaluate_reaction(r);
		if (r != Reaction::none) {
			c.print(r.description);
		}

	} while (c.last_input() != "quit game");

}

File_Writer& File_Writer::operator<<(const char* s) {
	if (_escape)
		*this << '"';
	for (char c = *s; c = *s, c!='\0'; ++s) {
		if (_escape && c == '"') {
			*this << '\\';
			*this << '"';
		} else {
			*this << c;
		}
	}
	if (_escape)
		*this << '"';
	return *this;
}

File_Writer& File_Writer::operator<<(const string& s) {
	*this << s.c_str();
	return *this;
}
template <class K, class V>
File_Writer& File_Writer::operator<<(const pair<K, V>& p) {
	*this << "Pair {first = " << p.first << "; second = " << p.second << '}';
	return *this;
}

template <class K, class V>
File_Writer& File_Writer::operator<<(const map<K, V>& m) {
	*this << "Map {";
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
	*this << escape << i.id << unescape;
	return *this;
}


File_Writer& File_Writer::operator<<(const Reaction& r) {
	*this << "Reaction {";
	*this << intend;

	*this << "description = " << escape << r.description << unescape;

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

	*this << "key = " << escape << a.key << unescape;

	if (!a.needs_context_vars_to_be.empty()) {
		*this << sep;
		*this << "needs_cxt_vars = ";
		*this << a.needs_context_vars_to_be;
	}

	if (!a.needs_items.empty()) {
		*this << sep;
		*this << "needs_items = ";
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

bool is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

bool is_delimiter(char c) {
	return c == ';' || c == '}';
}

bool is_upper(char c) {
	return c >= 65 && c <= 90;
}
bool is_lower(char c) {
	return c >= 97 && c <= 122;
}

// skips white space!
bool has_next_keyword(File_Reader& fr, const char* str, bool skip_key_word) {
	auto prev_pos = fr.tellg();
	bool success = true;
	char c;
	// compare strings
	while (fr.good() && *str != '\0') {
		c = fr.get();
		if (is_whitespace(c)) continue;
		if (c != *str)
			success = false;
		++str;
	}
	if (!skip_key_word)
		fr.seekg(prev_pos);
	return success;
}

string File_Reader::next_word_until(bool (*is_func) (const char c), bool putback) {
	char c = '\0';
	string tmp;
	while (this->good()) {
		c = this->get();
		if (is_func(c)) break;
		tmp += c;
	}

	if (c != '\0' && putback)
		this->putback(c);

	return tmp;
}

void cut_of_trailing_ws(string& s) {

	auto botws = s.length(); // begin_of_trailing_ws
	for ( ; botws > 0 && is_whitespace(s[botws -1]); --botws);

	auto bolws = botws; // begin_of_leading_ws
	for (bolws = 0; bolws < s.length() && is_whitespace(s[bolws]); ++bolws);

	if (bolws == 0 && botws != s.length()) {
		string tmp;

		for (auto i = bolws; i < botws; ++i) {
			tmp += s[i];
		}

		// cout << " 8< snip >8 " << (bolws + s.length() - botws) << endl;
		s = std::move(tmp);

	}
}


File_Reader& File_Reader::operator>>(vector<Block>& b) {

	char c = '\0';
	bool prev_layer_forbids_enum = this->layer_forbids_enum;
	this->layer_forbids_enum = false;

	while (this->good()) {
		Block tmp;
		bool fail = false;
		bool consume_close_bracket = false;

		// find beginning of someting
		*this >> c;
		this->putback(c);

		// type or value?
		if (is_upper(c)) { // assume type

			// type name
			tmp.type = next_word_until(is_whitespace, false);

			// find block beginn
			*this >> c;
			if (c == '{') {
				*this >> tmp.sub_blocks;
				consume_close_bracket = true;
			} else {
				fail = true;
			}

		} else if (is_lower(c)) { // assume value

			// keyword
			tmp.name = next_word_until(is_whitespace, false);

			// find assignment
			*this >> c;
			if (c == '=') { // read value

				// find beginning of value
				*this >> c;
				if (c == '{') { // list

					*this >> tmp.sub_blocks;
					consume_close_bracket = true;

				} else if (is_upper(c)) { // new Block

					this->putback(c);
					this->layer_forbids_enum = true;
					*this >> tmp.sub_blocks;
					this->layer_forbids_enum = false;

				} else { // plain value

					this->putback(c);
					tmp.value = next_word_until(is_delimiter, true);
				}
			} else {
				fail = true;
			}

		} else {

			// take as value
			tmp.value = next_word_until(is_delimiter, true);

		}

		if (fail) {
			this->setstate(ios::failbit);
			break;
		} else {
			if (!tmp.empty()) {
				cut_of_trailing_ws(tmp.type);
				cut_of_trailing_ws(tmp.name);
				cut_of_trailing_ws(tmp.value);
				// cout << tmp.type << "|" << tmp.name <<  "|" << tmp.value << "@" << endl;
				b.push_back(std::move(tmp));
			}
		}

		if (this->eof()) {
			break;
		}

		*this >> c;
		if (c == ';') continue;
		if (c == '}' && !consume_close_bracket) {
			this->putback(c);
		}
		if (prev_layer_forbids_enum) break;
		*this >> c;
		if (c != ';') {
			this->putback(c);
			break;
		}
	}

	this->layer_forbids_enum = prev_layer_forbids_enum;
	return *this;
}

bool read_escaped_string_to(const string& esc_str, string& dest) {
	string tmp;

	auto ci = esc_str.length();

	if (ci < 3) return false;
	if (esc_str[0] != '"' || esc_str[ci - 1] != '"') return false;

	bool escaped = false;
	for (ci=1; ci < esc_str.length() -1; ++ci) {
		if (esc_str[ci] == '"' && !escaped) break;
		if (esc_str[ci] == '\\') {escaped = true; continue;}
		tmp += esc_str[ci];
		escaped = false;
	}

	if (ci == esc_str.length() - 1) {
		dest = std::move(tmp);
		return true;
	}
	return false;
}

bool operator>>(Block& b, pair<string, string>& p) {
	pair<string, string> tmp;
	bool good = (b.type == "Pair");

	good |= b.sub_blocks.size() == 2;
	good |= !b.sub_blocks[0].value.empty();
	good |= !b.sub_blocks[1].value.empty();

	if (good) {
		tmp.first  = b.sub_blocks[0].value;
		tmp.second = b.sub_blocks[1].value;
	}

	if (good)
		p = std::move(tmp);

	return good;

}


bool operator>>(Block& b, map<string, string>& m) {
	map<string, string> tmp;
	bool good = (b.type == "Map");

	if (good) {
		for (auto it=b.sub_blocks.begin(); it!=b.sub_blocks.end(); ++it) {
			pair<string, string> tmp_pair;
			good |= (*it >> tmp_pair);
			if (good) tmp[tmp_pair.first] = tmp_pair.second;
		}
	}

	if (good)
		m = std::move(tmp);

	return good;
	
}

bool operator>>(Block& b, Item& i) {
	Item tmp;

	bool good = read_escaped_string_to(b.value, tmp.id);

	if (good)
		i = std::move(tmp);

	return good;
}


bool operator>>(Block& b, Reaction& r) {
	Reaction tmp;
	bool good = (b.type == "Reaction");

	for (int i=0; good && i < b.sub_blocks.size(); ++i) {

		if (b.sub_blocks[i].name == "description") {
			good |= read_escaped_string_to(b.sub_blocks[i].value, tmp.description);
		}

		if (b.sub_blocks[i].name == "set_cxt_vars") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				pair<string, string> tmp_pair;
				good |= (*it >> tmp_pair);
				if (good) tmp.set_context_vars.push_back(tmp_pair);
			}
		}

		if (b.sub_blocks[i].name == "give_items") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				Item tmp_itm;
				good |= (*it >> tmp_itm);
				if (good) tmp.give_items.push_back(tmp_itm);
			}
		}

		if (b.sub_blocks[i].name == "take_items") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				Item tmp_itm;
				good |= (*it >> tmp_itm);
				if (good) tmp.take_items.push_back(tmp_itm);
			}
		}
	}

	if (good)
		r = std::move(tmp);

	return good;
}

bool operator>>(Block& b, Action& a) {
	Action tmp;
	bool good = (b.type == "Action");

	for (int i=0; good && i < b.sub_blocks.size(); ++i) {

		if (b.sub_blocks[i].name == "key") {
			good |= read_escaped_string_to(b.sub_blocks[i].value, tmp.key);
		}

		if (b.sub_blocks[i].name == "needs_cxt_vars") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				pair<string, string> tmp_pair;
				good |= (*it >> tmp_pair);
				if (good) tmp.needs_context_vars_to_be.push_back(tmp_pair);
			}
		}

		if (b.sub_blocks[i].name == "needs_items") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				Item tmp_itm;
				good |= (*it >> tmp_itm);
				if (good) tmp.needs_items.push_back(tmp_itm);
			}
		}

		if (b.sub_blocks[i].name == "reaction" && !b.sub_blocks[i].sub_blocks.empty()) {
			Reaction tmp_r;
			good |= (b.sub_blocks[i].sub_blocks[0] >> tmp_r);
			if (good) tmp.reaction = std::move(tmp_r);
		}

	}

	if (good)
		a = std::move(tmp);

	return good;
}

bool operator>>(Block& b, Inventory& i) {
	Inventory tmp;
	bool good = (b.type == "Inventory");


	good |= !b.sub_blocks.empty();

	if (good) {
		for (auto it=b.sub_blocks.begin(); it!=b.sub_blocks.end(); ++it) {
			Item tmp_itm;
			good |= (*it >> tmp_itm);
			if (good) tmp.items.push_back(tmp_itm);
		}
	}

	if (good)
		i = std::move(tmp);

	return good;
}

bool operator>>(Block& b, Context& c) {
	Context tmp;
	bool good = (b.type == "Context");


	for (int i=0; good && i < b.sub_blocks.size(); ++i) {

		if (b.sub_blocks[i].name == "actions") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				Action tmp_ac;
				good |= (*it >> tmp_ac);
				if (good) tmp.actions.push_back(tmp_ac);
			}
		}

		if (b.sub_blocks[i].name == "inventory" && !b.sub_blocks[i].sub_blocks.empty()) {
			Inventory tmp_inv;
			good |= (b.sub_blocks[i].sub_blocks[0] >> tmp_inv);
			if (good) tmp.inventory = std::move(tmp_inv);
		}

		if (b.sub_blocks[i].name == "cxt_vars" && !b.sub_blocks[i].sub_blocks.empty()) {
			map<string, string> tmp_map;
			good |= (b.sub_blocks[i].sub_blocks[0] >> tmp_map);
			if (good) tmp.context_vars = std::move(tmp_map);
		}
	}

	if (good)
		c = std::move(tmp);

	return good;
}


File_Reader& File_Reader::operator>>(Context& c) {
	vector<Block> bs;
	bool fail = true;

	*this >> bs;

	for (auto it=bs.begin(); it!=bs.end(); ++it) {
		if (it->type == "Context") {
			Context tmp_cxt;
			if (*it >> tmp_cxt) {
				fail = false;
				c = std::move(tmp_cxt);
			}
		}
	}

	if (fail)
		this->setstate(ios::failbit);

	return *this;
}

