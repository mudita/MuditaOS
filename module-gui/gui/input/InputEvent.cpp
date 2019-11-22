/*
 * @file InputEvent.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "InputEvent.hpp"
namespace gui {

InputEvent::InputEvent( const State& state, const uint32_t& keyCode, const uint32_t& keyChar,
		const uint32_t& pressTime, const uint32_t& releaseTime, bool cycle, uint32_t timeout ) :
	state{state},
	keyCode{static_cast<KeyCode>(keyCode)}
    {}
};

