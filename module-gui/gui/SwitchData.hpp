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
	SwitchData( const std::string& description ) : description{description} {};
	virtual ~SwitchData() {};
	virtual const std::string& getDescription() const { return description; };
	virtual void setDescription( const std::string desc ) { description = desc; };
};

} /* namespace gui */

#endif /* MODULE_APPS_SWITCHDATA_HPP_ */
