//
//  tests.cpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 09.01.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#include "tests.hpp"

using namespace std;

bool do_tests()
{
	// Various Tests

	// basic setup
	Console c {cin, cout};
	Context cxt;

	// simple world
	/*
	cxt.context_vars["door"] = "closed";
	cxt.inventory.items.push_back({"stick"});
	cxt.actions.push_back({"hello", {}, {}, {"no respond"}});
	cxt.actions.push_back({"open door",  {{"door", "closed"}}, {}, {"crunching noise", {{"door", "opend" }}, {}}});
	cxt.actions.push_back({"close door", {{"door", "opend" }}, {}, {"crunching noise", {{"door", "closed"}}, {}}});
	cxt.actions.push_back({"grab stone", {}, {}, {"grabbed stone", {}, {{"stone"}}, {}}});
	cxt.actions.push_back({"throw stone", {}, {{"stone"}}, {"off it goes", {}, {}, {{"stone"}}}});
	return true;
	 */

	// IO tests
	/*
	File_Writer fw1 {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/test_save.txt"};
	fw1 << cxt;
	Inventory i3;
	Context cxt3;
	//
	File_Reader fr4 {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/test_save.txt"};
	vector<Block> bs;
	fr4 >> bs;
	Context cxt4;
	bs[0] >> cxt4;
	File_Writer fw4 {"/Users/Jens/Datein/Xcode/PenAndConsole/PenAndConsole/tmp/test_save.txt"};
	fw4 << cxt4;
	return true;
	 */

	return false;
}