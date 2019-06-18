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

namespace app {

//class template that stores information that was sent along with switch message
class SwitchData {
public:

	SwitchData() {};
	virtual ~SwitchData() {};
};

} /* namespace app */

#endif /* MODULE_APPS_SWITCHDATA_HPP_ */
