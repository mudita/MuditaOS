#pragma once

#include "../messages/AudioMessage.hpp"
#include <optional>
#include "Audio/decoder/decoder.hpp"

class Service;

class AudioServiceAPI {
public:
    static audio::RetCode PlaybackStart(sys::Service* serv,const std::string& fileName);
    static audio::RetCode RecordingStart(sys::Service* serv,const std::string& fileName);
    static audio::RetCode RoutingStart(sys::Service* serv);
    static audio::RetCode RoutingRecordCtrl(sys::Service* serv,bool enable);
    static audio::RetCode RoutingMute(sys::Service* serv,bool enable);
    static audio::RetCode RoutingSpeakerPhone(sys::Service* serv,bool enable);
    static audio::RetCode Stop(sys::Service* serv);
    static audio::RetCode Pause(sys::Service* serv);
    static audio::RetCode Resume(sys::Service* serv);
    static std::optional<audio::Tags> GetFileTags(sys::Service* serv,const std::string& fileName);
    static audio::RetCode SetOutputVolume(sys::Service* serv,const audio::Volume vol);
    static audio::RetCode SetInputGain(sys::Service* serv,const audio::Gain gain);
};
