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

    int32_t Play();

    int32_t Stop();

    int32_t Pause();

    int32_t Resume();

    uint32_t GetPosition();

    int32_t SwitchProfile(const Profile* prof);

private:
    std::unique_ptr<decoder> dec;
    std::unique_ptr<bsp::AudioDevice> audioDevice;
    const Profile* profile;
    State state = State ::Idle;
};


#endif //PUREPHONE_PLAYBACK_HPP
