/*
 * @file InputEvent.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_
#define MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_

#include <sstream>
#include <cstdint>

namespace gui {

class InputEvent {
public:
	enum class State{
		Undefined,
		keyPressed,
		keyReleasedShort,
		keyReleasedLong
	};

	State state = State::keyPressed;
	//
	uint32_t keyCode = 0;
	uint32_t keyChar = 0;
	uint32_t keyPressTime = 0;
	uint32_t keyRelaseTime = 0;
	uint32_t timeout = 0;

	InputEvent() {};
	InputEvent( const State& state, const uint32_t& code, const uint32_t& key, const uint32_t& pressTime, const uint32_t& releaseTime, uint32_t timeout = 0 );
	virtual ~InputEvent(){};

	std::string to_string() const{
		std::stringstream ss;
		ss<<"state: ";
		if( state == State::Undefined ) ss<<"Undefined";
		else if( state == State::keyPressed ) ss<<"Key Pressed";
		else if( state == State::keyReleasedShort ) ss<<"Key Released Short";
		else if( state == State::keyReleasedLong ) ss<<"Key Released Long";

		ss<<" key code: ";
		ss<<keyCode;
		ss<<" key char: ";
		ss<<keyChar;
		ss<<" press time: ";
		ss<<keyPressTime;
		ss<<" release time: ";
		ss<<keyRelaseTime;
		ss<<" timeout: ";
		ss<<timeout;

		return ss.str();
	}
};

} //namespace gui

#endif /* MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_ */
