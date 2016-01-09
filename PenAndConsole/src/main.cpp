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

#include "tests.hpp"


using namespace std;

//! @return true if string begins with exactly this characters
bool begins_with(const std::string& str, const std::string& beg);
bool is_valid_file_name(const string& str);

void print_game_stats(Console& c, Context& cxt);
void print_debug_info(Console&c, Context& cxt);
void print_possible_actions(Console& c, Context& cxt);
void print_inventory(Console& c, Context& cxt);

int main(int argc, const char * argv[]) {

	if (do_tests()) return EXIT_SUCCESS;

	// fundamental setup
	Console c {cin, cout};
	Context cxt;

	// main loop
	bool show_possible_actions_permanently = false;
	const std::string path_save = "/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/";
	do {

		// whait for next line of input
		c.aquire_input();

		// check for standart commands first:
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
			print_game_stats(c, cxt);
		}

		else if (c.last_input() == "show debug info") {
			c.ghost_write({"@show_debug_info"});
			print_debug_info(c, cxt);

		}

		else if (c.last_input() == "show possible actions") { // except hidden actions
			c.ghost_write({"@show_possible_actions"});
			print_possible_actions(c, cxt);
		}

		else if (c.last_input() == "toggle pin show possible actions") {
			show_possible_actions_permanently = !show_possible_actions_permanently;
		}

		else if (c.last_input() == "show inventory") {
			c.ghost_write({"@show_inventory"});
			print_inventory(c, cxt);
		}


		// execute action and print narrative description (if possible)
		Action& ac = cxt.get_action(c.last_input());
		cxt.evaluate_action(ac);
		if (ac.reaction.has_narrative_description()) {
			c.print(ac.reaction.narrative_description);
		}

		if (show_possible_actions_permanently) {
			print_possible_actions(c, cxt);
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

void print_game_stats(Console& c, Context& cxt) {
	c.costream() << endl
	<< "\t   -==- STATS -==- " << endl
	<< "\tactions loaded:     " << cxt.actions.size() << endl
	<< "\titmes in inventory: " << cxt.inventory.items.size() << endl
	<< "\tcontext vars:       " << cxt.context_vars.size() << endl <<endl;
}

void print_debug_info(Console&c, Context& cxt) {
	c.costream() << endl << "items in inventory (" << cxt.inventory.items.size() << "): ";
	bool first = true;
	for (auto it=cxt.inventory.items.begin(); it!=cxt.inventory.items.end(); ++it) {
		if (!first) c.costream() << " ~ ";
		first = false;
		c.costream() << it->id;
	}
	c.costream() << endl;
	c.costream() << endl << "actions (" << cxt.actions.size() << "):";
	c.costream() << endl << "\t key @[hidden;trigger_once_only;was_triggered]{needs items | forbids items | needs context var to be}"
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
		for (auto it2=it->forbids_items.begin(); it2!=it->forbids_items.end(); ++it2) {
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
		c.costream() << endl << "\t -> " << it->reaction.narrative_description << " @{";
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
	c.costream() << endl << "context vars (" << cxt.context_vars.size() << "): ";
	first = true;
	for (auto it=cxt.context_vars.begin(); it!=cxt.context_vars.end(); ++it) {
		c.costream() << endl << '\t';
		first = false;
		c.costream() << it->first << " : " << it->second;
	}
	c.costream() << endl << endl;
}

void print_possible_actions(Console& c, Context& cxt) {
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

void print_inventory(Console& c, Context& cxt) {
	bool first = true;
	for (auto it=cxt.inventory.items.begin(); it!=cxt.inventory.items.end(); ++it) {
		if (!first) c.costream() << " ~ ";
		first = false;
		c.costream() << it->id;
	}
	c.costream() << endl;
}
