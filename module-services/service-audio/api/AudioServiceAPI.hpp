/*
 *  @file AudioSerivceAPI.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_AUDIOSERIVCEAPI_HPP
#define PUREPHONE_AUDIOSERVICEAPI_HPP

#include "../messages/AudioMessage.hpp"
#include <optional>
#include "Audio/decoder/decoder.hpp"

class Service;

class AudioServiceAPI {
public:
    static RetCode PlaybackStart(sys::Service* serv,const std::string& fileName);
    static RetCode RecordingStart(sys::Service* serv,const std::string& fileName);
    static RetCode Stop(sys::Service* serv);
    static RetCode Pause(sys::Service* serv);
    static RetCode Resume(sys::Service* serv);
    static std::optional<Tags> GetFileTags(sys::Service* serv,const std::string& fileName);
    static RetCode SetOutputVolume(sys::Service* serv,const Volume vol);
    static RetCode SetInputGain(sys::Service* serv,const Gain gain);
};

#endif //PUREPHONE_AUDIOSERIVCEAPI_HPP
