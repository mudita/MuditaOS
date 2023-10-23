﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AudioMessage.hpp>
#include <AudioServiceAPI.hpp>
#include <ServiceAudio.hpp>

#include <Audio/decoder/Decoder.hpp>
#include <log/log.hpp>
#include <system/Common.hpp>

#include <utility>

namespace sys
{
    class Service;
} // namespace sys

using namespace audio;
namespace AudioServiceAPI
{
    namespace
    {
        auto SendAudioRequest(sys::Service *serv, std::shared_ptr<AudioMessage> msg)
        {
            LOG_DEBUG("Message type: %s", magic_enum::enum_name(msg->type).data());
            auto ret = serv->bus.sendUnicastSync(msg, service::name::audio, sys::BusProxy::defaultTimeout);
            if (ret.first == sys::ReturnCodes::Success) {
                if (auto resp = std::dynamic_pointer_cast<AudioResponseMessage>(ret.second)) {
                    LOG_DEBUG("Message processed");
                    return resp;
                }
                LOG_ERROR("Response type not AudioResponseMessage, actual type: %s",
                          magic_enum::enum_name(ret.second->type).data());
                return std::make_shared<AudioResponseMessage>(audio::RetCode::Failed);
            }
            LOG_ERROR("Command %s Failed with %d error",
                      magic_enum::enum_name(msg->type).data(),
                      static_cast<int>(ret.first));
            return std::make_shared<AudioResponseMessage>(audio::RetCode::Failed);
        }
    } // namespace

    bool PlaybackStart(sys::Service *serv, const audio::PlaybackType &playbackType, const std::string &fileName)
    {
        auto msg = std::make_shared<AudioStartPlaybackRequest>(fileName, playbackType);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool RecordingStart(sys::Service *serv, const std::string &fileName)
    {
        auto msg = std::make_shared<AudioStartRecorderRequest>(fileName);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool RoutingStart(sys::Service *serv)
    {
        auto msg = std::make_shared<AudioStartRoutingRequest>();
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Stop(sys::Service *serv, const std::vector<audio::PlaybackType> &stopVec)
    {
        if (stopVec.empty()) {
            return true;
        }
        auto msg = std::make_shared<AudioStopRequest>(stopVec);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Stop(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioStopRequest>(token);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool StopAll(sys::Service *serv)
    {
        auto msg = std::make_shared<AudioStopRequest>();
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Pause(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioPauseRequest>(token);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool Resume(sys::Service *serv, const audio::Token &token)
    {
        auto msg = std::make_shared<AudioResumeRequest>(token);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool SendEvent(sys::Service *serv, std::shared_ptr<audio::Event> evt)
    {
        auto msg = std::make_shared<AudioEventRequest>(std::move(evt));
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool SendEvent(sys::Service *serv, audio::EventType eType, audio::Event::DeviceState state)
    {
        auto msg = std::make_shared<AudioEventRequest>(eType, state);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    std::string GetSetting(sys::Service *serv, audio::Setting setting, audio::PlaybackType playbackType)
    {
        auto msg  = std::make_shared<AudioGetSetting>(playbackType, setting);
        auto resp = SendAudioRequest(serv, msg);
        if (resp->retCode == RetCode::Success) {
            return resp->val;
        }

        return "";
    }

    std::optional<audio::SettingState> GetSettingState(sys::Service *serv,
                                                       audio::Setting setting,
                                                       audio::PlaybackType playbackType)
    {
        auto settingValue = GetSetting(serv, setting, playbackType);
        if (!settingValue.empty()) {
            return settingValue != "0" ? audio::SettingState::Enabled : audio::SettingState::Disabled;
        }

        return std::nullopt;
    }

    template <typename T>
    audio::RetCode SetSetting(sys::Service *serv,
                              audio::Setting setting,
                              const T value,
                              audio::PlaybackType playbackType)
    {
        auto msg = std::make_shared<AudioSetSetting>(playbackType, setting, std::to_string(value));

        return SendAudioRequest(serv, msg)->retCode;
    }

    template <>
    audio::RetCode SetSetting<std::string>(sys::Service *serv,
                                           audio::Setting setting,
                                           const std::string str,
                                           audio::PlaybackType playbackType)
    {
        auto msg = std::make_shared<AudioSetSetting>(playbackType, setting, str);

        return SendAudioRequest(serv, msg)->retCode;
    }

    audio::RetCode SetVibrationSetting(sys::Service *serv,
                                       audio::SettingState settingState,
                                       audio::PlaybackType playbackType)
    {
        return SetSetting(
            serv, audio::Setting::EnableVibration, settingState == audio::SettingState::Enabled, playbackType);
    }

    audio::RetCode SetSystemSoundSetting(sys::Service *serv,
                                         audio::SettingState settingState,
                                         audio::PlaybackType playbackType)
    {
        return SetSetting(
            serv, audio::Setting::IsSystemSound, settingState == audio::SettingState::Enabled, playbackType);
    }

    audio::RetCode SetSoundSetting(sys::Service *serv,
                                   audio::SettingState settingState,
                                   audio::PlaybackType playbackType)
    {
        return SetSetting(
            serv, audio::Setting::EnableSound, settingState == audio::SettingState::Enabled, playbackType);
    }

    std::optional<audio::SettingState> GetVibrationSetting(sys::Service *serv, audio::PlaybackType playbackType)
    {
        return GetSettingState(serv, audio::Setting::EnableVibration, playbackType);
    }

    audio::RetCode SetVibrationLevelSetting(sys::Service *serv, const audio::VibrationLevel vibrationLevel)
    {
        return SetSetting(serv, audio::Setting::VibrationLevel, std::to_string(vibrationLevel), PlaybackType::System);
    }

    std::optional<audio::VibrationLevel> GetVibrationLevelSetting(sys::Service *serv)
    {
        std::optional<audio::VibrationLevel> volume;
        try {
            return static_cast<audio::VibrationLevel>(
                std::stoi(GetSetting(serv, audio::Setting::VibrationLevel, PlaybackType::System)));
        }
        catch (const std::logic_error &e) {
            LOG_ERROR("Setting vibration level failed: %s", e.what());
            return std::nullopt;
        }
    }

    std::optional<audio::SettingState> GetSystemSoundSetting(sys::Service *serv, audio::PlaybackType playbackType)
    {
        return GetSettingState(serv, audio::Setting::IsSystemSound, playbackType);
    }

    std::optional<audio::SettingState> GetSoundSetting(sys::Service *serv, audio::PlaybackType playbackType)
    {
        return GetSettingState(serv, audio::Setting::EnableSound, playbackType);
    }

    audio::RetCode SetSound(sys::Service *serv, const std::string &filePath, audio::PlaybackType playbackType)
    {
        return SetSetting(serv, audio::Setting::Sound, filePath, playbackType);
    }

    std::string GetSound(sys::Service *serv, audio::PlaybackType playbackType)
    {
        return GetSetting(serv, audio::Setting::Sound, playbackType);
    }

    audio::RetCode SetVolume(sys::Service *serv, const audio::Volume vol, audio::PlaybackType playbackType)
    {
        return SetSetting(serv, audio::Setting::Volume, std::to_string(vol), playbackType);
    }

    std::optional<audio::Volume> GetVolume(sys::Service *serv, audio::PlaybackType playbackType)
    {
        try {
            return static_cast<audio::Volume>(std::stoi(GetSetting(serv, audio::Setting::Volume, playbackType)));
        }
        catch (const std::logic_error &e) {
            LOG_ERROR("Cannot get a volume: %s", e.what());
            return std::nullopt;
        }
    }

    bool KeyPressed(sys::Service *serv, const int step)
    {
        auto msg = std::make_shared<AudioKeyPressedRequest>(step);
        return serv->bus.sendUnicast(msg, service::name::audio);
    }

    bool BluetoothA2DPVolumeChanged(sys::Service *serv, const std::uint8_t volume)
    {
        return serv->bus.sendUnicast(std::make_shared<A2DPDeviceVolumeChanged>(volume), service::name::audio);
    }

    bool BluetoothHSPVolumeChanged(sys::Service *serv, const std::uint8_t volume)
    {
        return serv->bus.sendUnicast(std::make_shared<HSPDeviceVolumeChanged>(volume), service::name::audio);
    }
    bool BluetoothHFPVolumeChanged(sys::Service *serv, const std::uint8_t volume)
    {
        return serv->bus.sendUnicast(std::make_shared<HFPDeviceVolumeChanged>(volume), service::name::audio);
    }
} // namespace AudioServiceAPI
