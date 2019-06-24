/*
 * @file Profile.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_INPUT_PROFILE_HPP_
#define MODULE_GUI_GUI_INPUT_PROFILE_HPP_

#include <string>
#include <cstdint>
#include <vector>
#include <map>

namespace gui {

/*
 *
 */
class KeyProfile {
	//defines whether button can be pressed multiple times to get different characters.
	bool cyclic;
	//time to trigger long press timeout in keyboard's driver
	//0 means that there is no timeout for holding the key
	uint32_t timeout = 0;
	uint32_t mapSize = 0;
	std::vector<uint32_t> map;
public:

	KeyProfile();
	virtual ~KeyProfile();
};

/*
 *
 */
class Profile {
	std::string name;
	std::map<uint32_t, KeyProfile*> keys;
public:
	Profile();
	virtual ~Profile();

	void setName( std::string ) { this->name = name; };
	std::string getName(){ return name; };
};

} /* namespace gui */

#endif /* MODULE_GUI_GUI_INPUT_PROFILE_HPP_ */
