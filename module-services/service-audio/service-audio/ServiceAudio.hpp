﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioMessage.hpp"

#include <Audio/Audio.hpp>
#include <Audio/AudioMux.hpp>
#include <MessageType.hpp>
#include <Service/Service.hpp>
#include <Utils.hpp>

#include <service-db/DBServiceAPI.hpp>
#include <service-db/QueryMessage.hpp>

#include <functional>

namespace settings
{
    class Settings;
}

class ServiceAudio : public sys::Service
{
  public:
    ServiceAudio();

    ~ServiceAudio();

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    static const char *serviceName;

  private:
    enum class VibrationType
    {
        None,
        OneShot,
        Continuous
    };

    audio::AudioMux audioMux;
    audio::AudioMux::VibrationStatus vibrationMotorStatus = audio::AudioMux::VibrationStatus::Off;
    std::unique_ptr<settings::Settings> settingsProvider;
    std::map<std::string, std::string> settingsCache;

    auto IsVibrationMotorOn()
    {
        return vibrationMotorStatus == audio::AudioMux::VibrationStatus::On;
    }

    auto AudioServicesCallback(const sys::Message *msg) -> std::optional<std::string>;

    auto HandleStart(const audio::Operation::Type opType,
                     const std::string                       = "",
                     const audio::PlaybackType &playbackType = audio::PlaybackType::None)
        -> std::unique_ptr<AudioResponseMessage>;
    auto HandleStop(const std::vector<audio::PlaybackType> &stopTypes, const audio::Token &token, bool &muted)
        -> std::unique_ptr<AudioResponseMessage>;
    auto HandleStop(const std::vector<audio::PlaybackType> &stopTypes, const audio::Token &token)
        -> std::unique_ptr<AudioResponseMessage>;
    auto HandleSendEvent(std::shared_ptr<audio::Event> evt) -> std::unique_ptr<AudioResponseMessage>;
    auto HandlePause(const audio::Token &token) -> std::unique_ptr<AudioResponseMessage>;
    auto HandlePause(std::optional<audio::AudioMux::Input *> input) -> std::unique_ptr<AudioResponseMessage>;
    auto HandleResume(const audio::Token &token) -> std::unique_ptr<AudioResponseMessage>;
    auto HandleGetFileTags(const std::string &fileName) -> std::unique_ptr<AudioResponseMessage>;
    void HandleNotification(const AudioNotificationMessage::Type &type, const audio::Token &token);
    auto HandleKeyPressed(const int step) -> std::unique_ptr<AudioKeyPressedResponse>;
    void VibrationUpdate(const audio::PlaybackType &type               = audio::PlaybackType::None,
                         std::optional<audio::AudioMux::Input *> input = std::nullopt);
    auto GetVibrationType(const audio::PlaybackType &type) -> VibrationType;

    auto IsVibrationEnabled(const audio::PlaybackType &type) -> bool;
    auto IsOperationEnabled(const audio::PlaybackType &plType, const audio::Operation::Type &opType) -> bool;
    constexpr auto IsResumable(const audio::PlaybackType &type) const -> bool;
    constexpr auto ShouldLoop(const std::optional<audio::PlaybackType> &type) const -> bool;
    auto IsBusy() -> bool;

    void setSetting(const audio::Setting &setting,
                    const std::string &value,
                    const audio::Profile::Type &profileType,
                    const audio::PlaybackType &playbackType);
    [[nodiscard]] std::string getSetting(const audio::Setting &setting,
                                         const audio::Profile::Type &profileType,
                                         const audio::PlaybackType &playbackType);

    const std::pair<audio::Profile::Type, audio::PlaybackType> getCurrentContext();

    void settingsChanged(const std::string &name, std::string value);
};
