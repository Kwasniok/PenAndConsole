//
//  Item.hpp
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef Item_hpp
#define Item_hpp

#include <string>

struct Item {
	std::string id;

	bool operator==(const Item& rhs) {
		return this->id == rhs.id;
	}
	bool operator!=(const Item& rhs) {
		return !(*this == rhs);
	}
};

#endif /* Item_hpp */
