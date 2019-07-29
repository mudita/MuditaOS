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
#include <functional>

#include "Common.hpp"
#include "Operation/Operation.hpp"

class Audio {
public:

    enum class State {
        Idle,
        Playback,
        Recording,
        Routing,
    };

    Audio(std::function<int32_t (uint32_t)> asyncCallback);

    //Events
    int32_t SendEvent(const Operation::Event evt, const EventData *data);

    //utilities
    Position GetPosition();
    State GetCurrentState() const {return currentState;}

    //Range 0-1
    int32_t SetOutputVolume(Volume vol);
    //Range -10 <-> +10
    int32_t SetInputGain(Gain gain);

    Volume GetOutputVolume(){return currentOperation->GetOutputVolume();}
    Gain GetInputGain(){return currentOperation->GetInputGain();}

    //TODO:M.P Set/Get inputGain/outputVolume for each profile

    //Operations
    int32_t Start(Operation::Type op,const char *fileName);
    int32_t Stop();
    int32_t Pause();
    int32_t Resume();


private:

    State currentState = State::Idle;
    std::unique_ptr<Operation> currentOperation;

    std::function<int32_t (uint32_t)> asyncCallback = nullptr;


};


#endif //PUREPHONE_AUDIO_HPP
