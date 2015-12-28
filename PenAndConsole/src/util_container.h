//
//  util_container.h
//  PenAndConsole
//
//  Created by Jens Kwasniok on 28.12.15.
//  Copyright © 2015 Jens Kwasniok. All rights reserved.
//

#ifndef util_container_h
#define util_container_h

template <class C, class K>
inline bool has(const C& container, const K& key) {
	return std::find(container.begin(), container.end(), key) != container.end();
}

#endif /* util_container_h */
