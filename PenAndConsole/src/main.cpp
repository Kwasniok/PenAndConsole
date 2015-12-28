//
//  main.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 23.12.15.
//  Copyright (c) 2015 Jens Kwasniok. All rights reserved.
//

#include <iostream>

#include "Console.hpp"
#include "Context.hpp"
#include "File_Writer.hpp"
#include "File_Reader.hpp"

using namespace std;

bool begins_with(const std::string& str, const std::string& beg);
bool is_valid_file_name(const string& str);

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

	const std::string path_save = "/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/";
	do {
		c.aquire_input();

		if (begins_with(c.last_input(), "load game ")) {

			std::string file_name = c.last_input().substr(10); // cut of "load game "
			if(is_valid_file_name(file_name)) {

				std::string path_save_file = path_save;
				path_save_file += file_name;
				path_save_file += ".txt";

				File_Reader fr {path_save_file.c_str()};

				Context tmp;
				if(fr.good())
					fr >> tmp;

				if (!fr.fail()) {
					c.print("loaded game");
					c.ghost_write({"@loaded_game"});
					cxt = std::move(tmp);
				} else {
					c.print("could not read file.");
				}

				fr.close();

			} else {
				c.print("only a-zA-Z0-9 is allowed as file name");
			}
		}

		else if (begins_with(c.last_input(), "save game ")) {

			std::string file_name = c.last_input().substr(10); // cut of "save game "
			if(is_valid_file_name(file_name)) {

				std::string path_save_file = path_save;
				path_save_file += file_name;
				path_save_file += ".txt";

				File_Writer fw {path_save_file.c_str()};

				if(fw.good())
					fw << cxt;

				if (fw.good()) {
					c.print("game saved");
					c.ghost_write({"@saved_game"});
				} else {
					c.print("game not saved due to file error!");
				}

				fw.close();

			} else {
				c.print("only a-zA-Z0-9 is allowed as file name");
			}
		}

		else if (c.last_input() == "show game stats") {
			c.costream() << endl
			<< "\t   -==- STATS -==- " << endl
			<< "\tactions loaded:     " << cxt.actions.size() << endl
			<< "\titmes in inventory: " << cxt.inventory.items.size() << endl
			<< "\tcontext vars:       " << cxt.context_vars.size() << endl <<endl;

		}

		else if (c.last_input() == "show debug info") {
			c.ghost_write({"@show_debug_info"});

			c.costream() << endl << "inventory: ";
			bool first = true;
			for (auto it=cxt.inventory.items.begin(); it!=cxt.inventory.items.end(); ++it) {
				if (!first) c.costream() << " ~ ";
				first = false;
				c.costream() << it->id;
			}
			c.costream() << endl;
			c.costream() << endl << "actions:";
			c.costream() << endl << "\t key @[hidden;trigger_once_only;was_triggered]{needs item | needs context var to be}"
						 << endl << "\t -> response @{gives itmes | takes items | set context vars to}" << endl;
			first = true;
			for (auto it=cxt.actions.begin(); it!=cxt.actions.end(); ++it) {
				c.costream() << endl << '\t';
				first = false;
				c.costream() << it->key << " @["
					<< it->hidden << ';'
					<< it->trigger_once_only << ';'
					<< it->was_triggered << "]";
				c.costream() << "{";
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

		else if (c.last_input() == "show possible actions") { // except hidden actions
			c.ghost_write({"@show_possible_actions"});

			bool first = true;
			for (auto it=cxt.actions.begin(); it!=cxt.actions.end(); ++it) {
				if (!it->hidden && cxt.is_possible_action(*it)) {
					if (!first) c.costream() << " ~ ";
					first = false;
					c.costream() << it->key;
				}
			}
			if (first)
				c.costream() << "~ no actions possible ~";
			c.costream() << endl;

		}

		else if (c.last_input() == "show inventory") {
			c.ghost_write({"@show_inventory"});
			
			bool first = true;
			for (auto it=cxt.inventory.items.begin(); it!=cxt.inventory.items.end(); ++it) {
				if (!first) c.costream() << " ~ ";
				first = false;
				c.costream() << it->id;
			}
			c.costream() << endl;
		}

		Action& ac = cxt.get_action(c.last_input());
		cxt.evaluate_action(ac);
		if (ac.reaction.has_description()) {
			c.print(ac.reaction.description);
		}

	} while (c.last_input() != "quit game");

}

bool begins_with(const std::string& str, const std::string& beg) {
	if (str.length() < beg.length()) return false;

	for (int i=0; i<beg.length(); ++i) {
		if (str[i] != beg[i]) return false;
	}

	return true;
}

bool is_valid_file_name(const string& str){
	for (int i=0; i<str.length(); ++i) {
		char c = str[i];
		if (!((c >= 48 && c <= 57) || // digits
			  (c >= 65 && c <= 90) || // A-Z
			  (c >= 97 && c <= 122)   // a-z
			))
			return false;
	}
	return !str.empty();
}
