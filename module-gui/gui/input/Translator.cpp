/*
 * @file Translator.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <algorithm>
#include "Translator.hpp"
#include "log/log.hpp"
#include "vfs.hpp"

namespace gui {

bool Translator::initialized = false;
std::string Translator::profilesFolder= "";
std::map< std::string, std::shared_ptr<gui::Profile> > Translator::profiles;

Translator::Translator() {
}

Translator::~Translator() {

}

std::vector<std::string> Translator::getProfilesList(std::string ext) {

	std::vector<std::string> profileFiles;

	LOG_INFO( "Scanning %s profiles folder: %s", ext.c_str(), profilesFolder.c_str());
	auto dirList = vfs.listdir(profilesFolder.c_str(), ext );

	for( vfs::DirectoryEntry ent : dirList ) {
		if( ent.attributes != vfs::FileAttributes::Directory) {}
			profileFiles.push_back( profilesFolder + "/" + ent.fileName );
	}

	LOG_INFO("Total number of profiles: %d", profileFiles.size());

	return profileFiles;
}

bool Translator::setProfile( std::string profileName ) {
	auto it = profiles.find( profileName );
	if( it == profiles.end() ) {
		currentProfile = nullptr;
		return false;
	}
	currentProfile = it->second;
	return true;
}

void Translator::loadProfile( std::string filepath ) {
	Profile* p = new Profile();
	p->load(filepath);
	profiles.insert( std::pair<std::string, Profile*>(p->getName(), p));
}

InputEvent Translator::translate( bool pressed, uint32_t keyCode,
		uint32_t pressTime, uint32_t releaseTime,
		bool longPress ) {


	//TODO if no profile is loaded use default one

	//return error input event if there is no current profile or failed to set default profile
	if( currentProfile == nullptr ) {
		lastEvent = InputEvent(gui::InputEvent::State::Undefined,0,0,0,0, false );
		return lastEvent;
	}

	//local state is Release or state is undefined
	if( state == State::Undefined || state == State::Released ) {

		//incoming event is about pressed key
		if( pressed ) {

			//find if this key is handled in currently loaded profile
			const KeyProfile* keyProfile = currentProfile->getKeyProfile( keyCode );
			if( keyProfile == nullptr) {
				lastEvent = InputEvent(gui::InputEvent::State::Undefined,0,0,0,0, false);
				return lastEvent;
			}

			bool cycle = false;
			//if there was previous key check if there is option for cyclic press
			if( state == State::Released ) {
				//if previous key supports cyclic behavior and current event key is the same as previously
				if( (prevKeyProfile->cyclic) && prevKeyCode == keyCode ) {
					//check if distance in time is less than specified for cycling
					if( pressTime - prevTime <= cycleTime ) {
						cycle = true;
						cyclePosition = (cyclePosition+1)%prevKeyProfile->chars.size();
					}
					else {
						cyclePosition = 0;
					}
				}
				else {
					cyclePosition = 0;
				}
			}
			else {
				cyclePosition = 0;
			}

			//assign local key code, char code and time of the event
			prevKeyCode = keyProfile->keyCode;
			prevKeyChar = keyProfile->chars[cyclePosition];
			prevTime = pressTime;

			//update profile of the last pressed
			prevKeyProfile = keyProfile;

			//change state to pressed
			state = State::Pressed;

			lastEvent = InputEvent(gui::InputEvent::State::keyPressed, keyCode, keyProfile->chars[cyclePosition], pressTime, 0, cycle, keyProfile->timeouts[cyclePosition] );

			return lastEvent;
		}
		else{
			LOG_WARN( "Discarding event, key was not pressed");
		}
	}
	else if(( state == State::Pressed ) && ( keyCode == prevKeyCode )){
		if( !pressed ) {

			prevKeyCode = keyCode;

			//change state to released
			state = State::Released;

			gui::InputEvent::State eventState = gui::InputEvent::State::keyReleasedShort;
			if( longPress )
				eventState = gui::InputEvent::State::keyReleasedLong;

			lastEvent = InputEvent( eventState, keyCode, prevKeyProfile->chars[cyclePosition], lastEvent.keyPressTime, releaseTime, lastEvent.cycle, prevKeyProfile->timeouts[cyclePosition] );
			return lastEvent;
		}
		else {
			LOG_WARN( "Discarding event, key was not released");
		}
	}

	lastEvent = InputEvent(gui::InputEvent::State::Undefined,0,0,0,0, false, 0 );

	return lastEvent;
}

bool Translator::init( std::string baseDirectory ) {
	//do not initialize again
	if( initialized )
		return true;

	profilesFolder = baseDirectory+"/profiles";
	std::vector<std::string> profileFiles = getProfilesList(".kprof");

	for( std::string mapName : profileFiles ) {
		loadProfile( mapName );
	}
	initialized = true;
	return (profileFiles.size()==profiles.size());
}

const InputEvent& Translator::getLastEvent() {
	return lastEvent;
}

} /* namespace gui */
