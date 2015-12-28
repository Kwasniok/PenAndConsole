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

		if (c.last_input() == "show possible actions") { // except hidden actions
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

		if (c.last_input() == "show inventory") {
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
