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
//module-bsp
#include "key_codes.hpp"

namespace gui {

enum class KeyCode {
	KEY_UNDEFINED = 0,
	KEY_LEFT = static_cast<int>(bsp::KeyCodes::JoystickLeft),
	KEY_UP = static_cast<int>(bsp::KeyCodes::JoystickUp),
	KEY_RIGHT = static_cast<int>(bsp::KeyCodes::JoystickRight),
	KEY_DOWN = static_cast<int>(bsp::KeyCodes::JoystickDown),
	KEY_LF = static_cast<int>(bsp::KeyCodes::FnLeft),
	KEY_RF = static_cast<int>(bsp::KeyCodes::FnRight),
	KEY_ENTER = static_cast<int>(bsp::KeyCodes::JoystickEnter),
	KEY_0 = static_cast<int>(bsp::KeyCodes::NumericKey0),
	KEY_1 = static_cast<int>(bsp::KeyCodes::NumericKey1),
	KEY_2 = static_cast<int>(bsp::KeyCodes::NumericKey2),
	KEY_3 = static_cast<int>(bsp::KeyCodes::NumericKey3),
	KEY_4 = static_cast<int>(bsp::KeyCodes::NumericKey4),
	KEY_5 = static_cast<int>(bsp::KeyCodes::NumericKey5),
	KEY_6 = static_cast<int>(bsp::KeyCodes::NumericKey6),
	KEY_7 = static_cast<int>(bsp::KeyCodes::NumericKey7),
	KEY_8 = static_cast<int>(bsp::KeyCodes::NumericKey8),
	KEY_9 = static_cast<int>(bsp::KeyCodes::NumericKey9),
	KEY_AST = static_cast<int>(bsp::KeyCodes::NumericKeyAst),
	KEY_PND = static_cast<int>(bsp::KeyCodes::NumericKeyPnd),
//	KEY_GREEN,
//	KEY_RED,
	KEY_VOLUP = static_cast<int>(bsp::KeyCodes::VolUp),
	KEY_VOLDN = static_cast<int>(bsp::KeyCodes::VolDown),
	KEY_TORCH = static_cast<int>(bsp::KeyCodes::Torch),
	SWITCH_UP = static_cast<int>(bsp::KeyCodes::SSwitchUp),
	SWITCH_MID = static_cast<int>(bsp::KeyCodes::SSwitchMid),
	SWITCH_DN = static_cast<int>(bsp::KeyCodes::SSwitchDown),
};

class InputEvent {
public:
	enum class State{
		Undefined        = 0x00,
		keyPressed       = 0x01,
		keyReleasedShort = 0x02,
		keyReleasedLong  = 0x04,
	};

	State state = State::keyPressed;
	KeyCode keyCode = KeyCode::KEY_UNDEFINED;
	uint32_t keyChar = 0;
	uint32_t keyPressTime = 0;
	uint32_t keyRelaseTime = 0;
	bool     cycle = false;
	uint32_t timeout = 0;

	InputEvent() {};
	InputEvent( const State& state, const uint32_t& code, const uint32_t& key, const uint32_t& pressTime, const uint32_t& releaseTime, bool cycle, uint32_t timeout = 0 );
	virtual ~InputEvent(){};

	std::string to_string() const{
		std::stringstream ss;
		ss<<"state: ";
		if( state == State::Undefined ) ss<<"Undefined";
		else if( state == State::keyPressed ) ss<<"Key Pressed";
		else if( state == State::keyReleasedShort ) ss<<"Key Released Short";
		else if( state == State::keyReleasedLong ) ss<<"Key Released Long";

		ss<<" key code: ";
		ss<< static_cast<int>(keyCode);
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
