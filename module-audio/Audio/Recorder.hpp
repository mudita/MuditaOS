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
#include <functional>

class Encoder;
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

    State GetState(){return state;}

    const Profile* GetProfile(){return profile;}

private:
    std::unique_ptr<Encoder> enc;
    std::unique_ptr<bsp::AudioDevice> audioDevice;
    const Profile* profile;
    State state = State ::Idle;
    std::function<int32_t (int32_t)> errorCallback = nullptr;
};


#endif //PUREPHONE_RECORDER_HPP
