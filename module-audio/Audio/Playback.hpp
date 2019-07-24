/*
 *  @file Playback.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PLAYBACK_HPP
#define PUREPHONE_PLAYBACK_HPP

#include <memory>
#include <optional>
#include <functional>

class decoder;
class Tags;
class Profile;

namespace bsp {
    class AudioDevice;
}


class Playback {
public:

    enum class State{
        Idle,
        Play,
        Pause
    };

    Playback(const char *file, const Profile* profile);
    virtual ~Playback();

    int32_t Play(std::function<int32_t ()> eofCallback);

    int32_t Stop();

    int32_t Pause();

    int32_t Resume();

    uint32_t GetPosition();

    State GetState(){return state;}

    const Profile* GetProfile(){return profile;}

    int32_t SwitchProfile(const Profile* prof);

private:
    std::unique_ptr<bsp::AudioDevice> audioDevice;
    std::unique_ptr<decoder> dec;
    const Profile* profile;
    State state = State ::Idle;
    std::function<int32_t ()> endOfFileCallback = nullptr;
};


#endif //PUREPHONE_PLAYBACK_HPP
