/*
 *  @file Recorder.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RECORDER_HPP
#define PUREPHONE_RECORDER_HPP


#include <memory>
#include <optional>

class encoder;
class Tags;
class Profile;

namespace bsp {
    class AudioDevice;
}


class Recorder {
public:

    enum class State{
        Idle,
        Recording,
        Pause
    };

    Recorder(const char *file, const Profile* profile);
    virtual ~Recorder();

    int32_t Start();

    int32_t Stop();

    int32_t Pause();

    int32_t Resume();


    int32_t SwitchProfile(const Profile* prof);

private:
    std::unique_ptr<decoder> dec;
    std::unique_ptr<bsp::AudioDevice> audioDevice;
    const Profile* profile;
    State state = State ::Idle;
};


#endif //PUREPHONE_RECORDER_HPP
