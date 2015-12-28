//
//  File_Writer.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#include "File_Writer.hpp"

File_Writer& File_Writer::operator<<(const bool b) {
	if (b)
		*this << "true";
	else
		*this << "false";
	return *this;
}

File_Writer& File_Writer::operator<<(const char c) {
	put(c);
	return *this;
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

File_Writer& File_Writer::operator<<(const std::string& s) {
	*this << s.c_str();
	return *this;
}

template <class K, class V>
File_Writer& File_Writer::operator<<(const std::pair<K, V>& p) {
	*this << "Pair {first = " << p.first << "; second = " << p.second << '}';
	return *this;
}

template <class K, class V>
File_Writer& File_Writer::operator<<(const std::map<K, V>& m) {
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
File_Writer& File_Writer::operator<<(const std::vector<T>& v) {
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

	bool prev_wrap = _wrap;
	*this << "description = " << unwrap << escape << r.description << unescape;
	_wrap = prev_wrap;

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

	if (a.hidden) {
		*this << sep;
		*this << "hidden = ";
		*this << a.hidden;
	}

	if (a.trigger_once_only) {
		*this << sep;
		*this << "trigger_once_only = ";
		*this << a.trigger_once_only;
	}

	if (a.was_triggered) {
		*this << sep;
		*this << "was_triggered = ";
		*this << a.was_triggered;
	}

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

File_Writer& nl (File_Writer& fw) {
	endl(fw);
	for (int i=0; i < fw.intendation_level; ++i)
		fw << '\t';
	return fw;
}

File_Writer& sep (File_Writer& fw) {
	fw << ';';
	if(fw._wrap)
		fw << nl;
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
