#pragma once

#include "../messages/AudioMessage.hpp"
#include <optional>
#include "Audio/decoder/decoder.hpp"

class Service;

namespace AudioServiceAPI
{
    audio::RetCode PlaybackStart(sys::Service *serv, const std::string &fileName);
    audio::RetCode RecordingStart(sys::Service *serv, const std::string &fileName);
    audio::RetCode RoutingStart(sys::Service *serv);
    audio::RetCode RoutingRecordCtrl(sys::Service *serv, bool enable);
    audio::RetCode RoutingMute(sys::Service *serv, bool enable);
    audio::RetCode RoutingSpeakerPhone(sys::Service *serv, bool enable);
    audio::RetCode Stop(sys::Service *serv);
    audio::RetCode Pause(sys::Service *serv);
    audio::RetCode Resume(sys::Service *serv);
    std::optional<audio::Tags> GetFileTags(sys::Service *serv, const std::string &fileName);
    audio::RetCode SetOutputVolume(sys::Service *serv, const audio::Volume vol);
    audio::RetCode GetOutputVolume(sys::Service *serv, audio::Volume &vol);
    audio::RetCode SetInputGain(sys::Service *serv, const audio::Gain gain);
    audio::RetCode GetInputGain(sys::Service *serv, audio::Gain &gain);
}; // namespace AudioServiceAPI
