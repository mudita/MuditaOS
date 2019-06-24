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

namespace gui {

class InputEvent {
public:
	enum class State{
		keyPressed,
		keyReleasedShort,
		keyReleasedLong
	};

	State state = State::keyPressed;
	//
	uint32_t code = 0;
	uint32_t key = 0;
	uint32_t keyPressTime = 0;
	uint32_t keyRelaseTime = 0;

	InputEvent( const State& state);
	virtual ~InputEvent(){};
};

} //namespace gui

#endif /* MODULE_GUI_GUI_INPUT_INPUTEVENT_HPP_ */
