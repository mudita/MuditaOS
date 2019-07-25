/*
 *  @file Audio.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_AUDIO_HPP
#define PUREPHONE_AUDIO_HPP

#include <memory>
#include <optional>

#include "Common.hpp"

class Operation;

class Audio {
public:

    enum class State {
        Idle,
        Playback,
        Recording,
        Routing,
    };

    enum class Event{
        HeadphonesPlugIn,
        HeadphonesUnplug,

    };

    Audio();

    //utilities
    Position GetPosition();

    //Playback

    int32_t PlaybackStart(const char *fileName);

    int32_t PlaybackStop();

    int32_t PlaybackPause();

    int32_t PlaybackResume();

    // Recording

    int32_t RecordingStart(const char *fileName);

    int32_t RecordingStop();

    int32_t RecordingPause();

    int32_t RecordingResume();


private:

    State currentState = State::Idle;
    std::unique_ptr<Operation> currentOperation;
};


#endif //PUREPHONE_AUDIO_HPP
