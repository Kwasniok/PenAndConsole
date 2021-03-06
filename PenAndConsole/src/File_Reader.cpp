//
//  File_Reader.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#include "File_Reader.hpp"

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


//! @return copied string with no leading or trailing white space
void cut_of_trailing_ws(std::string& s) {

	auto botws = s.length(); // begin_of_trailing_ws
	for ( ; botws > 0 && is_whitespace(s[botws -1]); --botws);

	auto bolws = botws; // begin_of_leading_ws
	for (bolws = 0; bolws < s.length() && is_whitespace(s[bolws]); ++bolws);

	if (bolws == 0 && botws != s.length()) {
		std::string tmp;

		for (auto i = bolws; i < botws; ++i) {
			tmp += s[i];
		}

		// cout << " 8< snip >8 " << (bolws + s.length() - botws) << endl;
		s = std::move(tmp);
		
	}
}

//! @return true if next characters until white space match
//! @waring skips white space!
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

//! If string matched format of an escaped string it removes sourrounding quotes and unecapes newlines etc. .
//! Otherwise the dest string remains untouched.
//! @return true if string was valid
bool read_escaped_string_to(const std::string& esc_str, std::string& dest) {
	std::string tmp;

	auto ci = esc_str.length();

	if (ci < 3) return false;
	if (esc_str[0] != '"' || esc_str[ci - 1] != '"') return false;

	bool escaped = false;
	for (ci=1; ci < esc_str.length() -1; ++ci) {
		if (escaped) {
			if (esc_str[ci] == '\\') tmp += '\\';
			if (esc_str[ci] == '"') tmp += '"';
			if (esc_str[ci] == 'n') tmp += '\n';
			escaped = false;
		} else {
			if (esc_str[ci] == '\\') escaped = true;
			else tmp += esc_str[ci];
		}
	}

	if (ci == esc_str.length() - 1) {
		dest = std::move(tmp);
		return true;
	}
	return false;
}

//! converts ecplitit 'true' and 'false' back to values
bool read_bool_to(const std::string& value, bool& dest) {
	if (value == "true") {dest = true; return true;}
	if (value == "false") {dest = false; return true;}
	return false;
}

bool operator>>(Block& b, std::pair<std::string, std::string>& p) {
	std::pair<std::string, std::string> tmp;
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

bool operator>>(Block& b, std::map<std::string, std::string>& m) {
	std::map<std::string, std::string> tmp;
	bool good = (b.type == "Map");

	if (good) {
		for (auto it=b.sub_blocks.begin(); it!=b.sub_blocks.end(); ++it) {
			std::pair<std::string, std::string> tmp_pair;
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

		if (b.sub_blocks[i].name == "narrative_description") {
			good |= read_escaped_string_to(b.sub_blocks[i].value, tmp.narrative_description);
		}

		if (b.sub_blocks[i].name == "set_cxt_vars") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				std::pair<std::string, std::string> tmp_pair;
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

		if (b.sub_blocks[i].name == "hidden") {
			good |= read_bool_to(b.sub_blocks[i].value, tmp.hidden);
		}

		if (b.sub_blocks[i].name == "trigger_once_only") {
			good |= read_bool_to(b.sub_blocks[i].value, tmp.trigger_once_only);
		}

		if (b.sub_blocks[i].name == "was_triggered") {
			good |= read_bool_to(b.sub_blocks[i].value, tmp.was_triggered);
		}

		if (b.sub_blocks[i].name == "needs_cxt_vars") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				std::pair<std::string, std::string> tmp_pair;
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

		if (b.sub_blocks[i].name == "forbids_items") {
			for (auto it=b.sub_blocks[i].sub_blocks.begin(); it!=b.sub_blocks[i].sub_blocks.end(); ++it) {
				Item tmp_itm;
				good |= (*it >> tmp_itm);
				if (good) tmp.forbids_items.push_back(tmp_itm);
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
			std::map<std::string, std::string> tmp_map;
			good |= (b.sub_blocks[i].sub_blocks[0] >> tmp_map);
			if (good) tmp.context_vars = std::move(tmp_map);
		}
	}

	if (good)
		c = std::move(tmp);
	
	return good;
}

//! @param is_function a function indication to stop when an a special character apperas
//! @param putback put return to previous position after leading
//! @warning skips leading white space
std::string next_word_until(File_Reader& fr, bool (*is_func) (const char c), bool putback) {
	char c = '\0';
	std::string tmp;
	while (fr.good()) {
		c = fr.get();
		if (is_func(c)) break;
		tmp += c;
	}

	if (c != '\0' && putback)
		fr.putback(c);

	return tmp;
}

File_Reader& File_Reader::operator>>(std::vector<Block>& b) {

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
			tmp.type = next_word_until(*this, is_whitespace, false);
			if (!this->good()) break;

			// find block beginn
			*this >> c;
			if (c == '{') {
				*this >> tmp.sub_blocks;
				if (!this->good()) break;
				consume_close_bracket = true;
			} else {
				fail = true;
			}

		} else if (is_lower(c)) { // assume value

			// keyword
			tmp.name = next_word_until(*this, is_whitespace, false);
			if (!this->good()) break;

			// find assignment
			*this >> c;
			if (c == '=') { // read value

				// find beginning of value
				*this >> c;
				if (c == '{') { // list

					*this >> tmp.sub_blocks;
					if (!this->good()) break;
					consume_close_bracket = true;

				} else if (is_upper(c)) { // new Block

					this->putback(c);
					this->layer_forbids_enum = true;
					*this >> tmp.sub_blocks;
					this->layer_forbids_enum = false;
					if (!this->good()) break;

				} else { // plain value

					this->putback(c);
					tmp.value = next_word_until(*this, is_delimiter, true);
					if (!this->good()) break;
				}
			} else {
				fail = true;
			}

		} else {

			// take as value
			tmp.value = next_word_until(*this, is_delimiter, true);
			if (!this->good()) break;

		}

		if (fail) {
			this->setstate(std::ios::failbit);
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

		/*
		if (this->eof()) {
			break;
		}
		*/

		// ';' at this position indicates more blocks on this level
		//     e.g. another item in a list or another variable
		// '}' is allways a delimiter
		//     e.g. end of list or instance
		*this >> c;
		if (c == ';') continue;
		if (c == '}' && !consume_close_bracket) {
			this->putback(c);
		}
		if (prev_layer_forbids_enum) break;

		// reaching EOF is not a fail
		// EOF can be used as final delimiter
		fail = this->fail();
		*this >> c;
		if (this->eof() && !fail) {
			this->clear(std::ios::eofbit);
			break;
		}

		if (c != ';') {
			this->putback(c);
			break;
		}
	}

	this->layer_forbids_enum = prev_layer_forbids_enum;
	return *this;
}

File_Reader& File_Reader::operator>>(Context& c) {
	std::vector<Block> bs;
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
		this->setstate(std::ios::failbit);
	
	return *this;
}
