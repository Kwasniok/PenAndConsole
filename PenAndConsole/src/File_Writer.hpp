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
	bool _wrap = false;
	bool _escape = false;

	File_Writer(const char* path) : std::ofstream(path) { }

	File_Writer& operator<<(File_Writer& (*pf) (File_Writer&)) {
		return pf(*this);
	}

	File_Writer& operator<<(std::ofstream& (*pf) (std::ofstream&)) {
		pf(*this);
		return *this;
	}

	File_Writer& operator<<(const char* str);
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
};

File_Writer& nl (File_Writer& fw);
File_Writer& sep (File_Writer& fw);
File_Writer& intend (File_Writer& fw);
File_Writer& unintend (File_Writer& fw);
File_Writer& wrap (File_Writer& fw);
File_Writer& unwrap (File_Writer& fw);
File_Writer& escape (File_Writer& fw);
File_Writer& unescape (File_Writer& fw);

#endif /* File_Writer_hpp */
