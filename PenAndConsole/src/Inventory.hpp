//
//  Inventory.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef Inventory_hpp
#define Inventory_hpp

#include <vector>

#include "Item.hpp"

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

#endif /* Inventory_hpp */
