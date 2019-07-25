/*
 *  @file Operation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 24.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_OPERATION_HPP
#define PUREPHONE_OPERATION_HPP

#include <memory>
#include <optional>
#include <functional>

#include "Audio/encoder/Encoder.hpp"
#include "Audio/Profiles/Profile.hpp"

class Operation {
public:

    enum class State{
        Idle,
        Active,
        Paused
    };

    enum class Type{
        Idle,
        Playback,
        Recorder,
        Router
    };

    using Position = float;

    Operation();
    virtual ~Operation(){}

    static std::optional<std::unique_ptr<Operation>> Create(Type t,const char* fileName,const Encoder::Format& frmt={});

    virtual int32_t Start(std::function<int32_t (uint32_t)> callback) = 0;

    virtual int32_t Stop() = 0;

    virtual int32_t Pause() = 0;

    virtual int32_t Resume() = 0;

    virtual int32_t SwitchProfile(const Profile::Type type) = 0;

    virtual Position GetPosition() = 0;

    State GetState(){return state;}

    const Profile GetProfile(){return profile;}

protected:

    Profile profile;
    std::vector<Profile > availableProfiles;
    State state = State ::Idle;
    std::function<int32_t (uint32_t)> eventCallback = nullptr;

    bool isInitialized = false;

    std::optional<Profile> GetProfile(const Profile::Type type);

};


#endif //PUREPHONE_OPERATION_HPP
