//
//  File_Writer.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef File_Writer_hpp
#define File_Writer_hpp

#include <fstream>
#include <vector>
#include <map>

#include "Context.hpp"

class File_Writer : public std::ofstream {
public:
	int intendation_level = 0;
	//! print intendations
	bool _wrap = false;
	//! print strings with surrounding quotationmarks and escape newlines etc.
	bool _escape = false;

	File_Writer(const char* path) : std::ofstream(path) { }


	//! boolean values are written explicitly as 'true' and 'false'
	File_Writer& operator<<(const bool b);
	File_Writer& operator<<(const char c);
	//! sensible to ecaping
	File_Writer& operator<<(const char* str);
	//! sensilbe to escaping
	File_Writer& operator<<(const std::string& str);
	template <class K, class V>
	File_Writer& operator<<(const std::pair<K, V>& p);
	template <class K, class V>
	File_Writer& operator<<(const std::map<K, V>& m);
	template <class T>
	File_Writer& operator<<(const std::vector<T>& v);
	File_Writer& operator<<(const Item& i);
	File_Writer& operator<<(const Reaction& r);
	File_Writer& operator<<(const Action& r);
	File_Writer& operator<<(const Inventory& r);
	File_Writer& operator<<(const Context& r);

	//! used for stream functions
	File_Writer& operator<<(File_Writer& (*pf) (File_Writer&)) {
		return pf(*this);
	}
	//! used for stream functions
	File_Writer& operator<<(std::ofstream& (*pf) (std::ofstream&)) {
		pf(*this);
		return *this;
	}
};

// stream functions:

//! add newline
File_Writer& nl (File_Writer& fw);
//! add separator as used between items in a list or after assignments
File_Writer& sep (File_Writer& fw);

//! begins intendation (including new line) <p> suppred by wrapping
//! @see intend, unintend, wrap, unwrap
File_Writer& intend (File_Writer& fw);
//! ends intendation (including new line)<p> suppred by wrapping
//! @see intend, unintend, wrap, unwrap
File_Writer& unintend (File_Writer& fw);
//! begins wrapping (allow intendation)
//! @see intend, unintend, wrap, unwrap
File_Writer& wrap (File_Writer& fw);
//! end wrapping (suppress intendation)
//! @see intend, unintend, wrap, unwrap
File_Writer& unwrap (File_Writer& fw);

//! begin escaping (when printing strings print  urrounding quotationmarks and escape newlines etc.)
//! @see unescape
File_Writer& escape (File_Writer& fw);
//! end escaping (all strings are printed one to one e.g. new lines become real newlines and are not escaped)
//! @see escape
File_Writer& unescape (File_Writer& fw);

#endif /* File_Writer_hpp */
