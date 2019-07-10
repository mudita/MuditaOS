/*
 * @file CallSwitchData.h
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_DATA_CALLSWITCHDATA_HPP_
#define MODULE_APPS_APPLICATION_CALL_DATA_CALLSWITCHDATA_HPP_

#include <string>
#include "SwitchData.hpp"

namespace app {

/*
 *
 */
class CallSwitchData: public gui::SwitchData {
public:
	enum class Type {
		UNDEFINED,
		NUMBER,
		INCOMMING_CALL
	};
protected:
	Type type = Type::UNDEFINED;
public:
	CallSwitchData( app::CallSwitchData::Type type ) : type{ type } {};
	virtual ~CallSwitchData(){};

	const Type& getType() const { return type; };
};

class CallNumberData: public CallSwitchData {
protected:
	std::string phoneNumber;
public:
	CallNumberData( std::string number ) : CallSwitchData( app::CallSwitchData::Type::NUMBER ), phoneNumber{ number }{};
	virtual ~CallNumberData(){};

	const std::string& getPhoneNumber() const { return phoneNumber; };
};

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALL_DATA_CALLSWITCHDATA_HPP_ */
