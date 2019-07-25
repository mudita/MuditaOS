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

class Profile;

namespace bsp {
    class AudioDevice;
}


class Operation {
public:

    enum class State{
        Idle,
        Active,
        Paused
    };

    using Position = float;

    Operation(const Profile* prof);

    virtual int32_t Start(std::function<int32_t (uint32_t)> callback) = 0;

    virtual int32_t Stop() = 0;

    virtual int32_t Pause() = 0;

    virtual int32_t Resume() = 0;

    virtual int32_t SwitchProfile(const Profile* prof) = 0;

    virtual Position GetPosition() = 0;

    State GetState(){return state;}

    const Profile* GetProfile(){return profile;}

protected:
    const Profile* profile;
    State state = State ::Idle;
    std::function<int32_t (uint32_t)> eventCallback = nullptr;

};


#endif //PUREPHONE_OPERATION_HPP
