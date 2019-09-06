/*
 * @file SwitchData.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 15 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_SWITCHDATA_HPP_
#define MODULE_APPS_SWITCHDATA_HPP_

namespace gui {

//class template that stores information that was sent along with switch message
class SwitchData {
protected:
	std::string description = "";
public:

	SwitchData() {};
	virtual ~SwitchData() {};
	virtual const std::string& getDescription() { return description; };
};

} /* namespace gui */

#endif /* MODULE_APPS_SWITCHDATA_HPP_ */
