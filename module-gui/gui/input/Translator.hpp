/*
 * @file Translator.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_INPUT_TRANSLATOR_HPP_
#define MODULE_GUI_GUI_INPUT_TRANSLATOR_HPP_

#include <string>
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include "bsp/keyboard/key_codes.hpp"
#include "Profile.hpp"
#include "InputEvent.hpp"

namespace gui {

/*
 *
 */
class Translator {
public:
	enum class State{
		Undefined,
		Pressed,
		Released,
	};
	static const uint32_t cycleTime = 500;
protected:

	//bsp code of the pressed or released button
	uint32_t prevKeyCode = 0;
	//character assigned to pressed key by the selected profile;
	uint32_t prevKeyChar = 0;
	//time of the last event received from system. This is time when button was pressed.
	uint32_t prevTime = 0;
	//internal state of the translation.
	State state = State::Undefined;
	//last cycle index for the key
	uint32_t cyclePosition = 0;
	const KeyProfile* prevKeyProfile = nullptr;

	static bool initialized;
	static std::string profilesFolder;
	std::shared_ptr<gui::Profile> currentProfile = nullptr;
	static std::map< std::string, std::shared_ptr<gui::Profile> > profiles;

	InputEvent lastEvent;

	void loadProfile( std::string filepath );
	std::vector<std::string> getProfilesList(std::string ext);
public:
	Translator();
	virtual ~Translator();

	bool init( std::string baseDirectory );
	/**
	 * @brief Sets profile used by application. This will reset state of the last received keyboard key.
	 * @return true if profile switch was successful, false otherwise.
	 */
	bool setProfile( std::string profileName );

	const InputEvent& getLastEvent();

	InputEvent translate( bool pressed, uint32_t keyCode, uint32_t pressTime, uint32_t releaseTime, bool longPress = false );
};

} /* namespace gui */

#endif /* MODULE_GUI_GUI_INPUT_TRANSLATOR_HPP_ */
