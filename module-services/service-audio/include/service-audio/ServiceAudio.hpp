// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioMessage.hpp"
#include "service-audio/AudioServiceName.hpp"

#include <Audio/Audio.hpp>
#include <Audio/AudioMux.hpp>
#include <service-db/DBServiceName.hpp>
#include <service-db/QueryMessage.hpp>
#include <Service/Service.hpp>

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

    void ProcessCloseReason(sys::CloseReason closeReason) override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

  private:
    enum class VibrationType
    {
        None,
        OneShot,
        Continuous
    };

    audio::AudioMux audioMux;
    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    audio::AudioMux::VibrationStatus vibrationMotorStatus = audio::AudioMux::VibrationStatus::Off;
    std::unique_ptr<settings::Settings> settingsProvider;
    std::map<std::string, std::string> settingsCache;
    bool bluetoothA2DPConnected         = false;
    bool bluetoothVoiceProfileConnected = false;

    std::map<VibrationType, std::list<audio::PlaybackType>> vibrationMap = {
        {VibrationType::None, {}},
        {VibrationType::Continuous, {audio::PlaybackType::CallRingtone, audio::PlaybackType::Alarm}},
        {VibrationType::OneShot, {audio::PlaybackType::Notifications, audio::PlaybackType::TextMessageRingtone}}};

    auto IsVibrationMotorOn()
    {
        return vibrationMotorStatus == audio::AudioMux::VibrationStatus::On;
    }

    auto AudioServicesCallback(const sys::Message *msg) -> std::optional<std::string>;

    auto HandleStart(const audio::Operation::Type opType,
                     const std::string                       = "",
                     const audio::PlaybackType &playbackType = audio::PlaybackType::None)
        -> std::unique_ptr<AudioResponseMessage>;
    auto HandleStop(const std::vector<audio::PlaybackType> &stopTypes, const audio::Token &token)
        -> std::unique_ptr<AudioResponseMessage>;

    enum class StopReason
    {
        Eof,
        Other
    };

    auto StopInput(audio::AudioMux::Input *input, StopReason stopReason = StopReason::Other) -> audio::RetCode;
    auto HandleSendEvent(std::shared_ptr<audio::Event> evt) -> std::unique_ptr<AudioResponseMessage>;
    auto HandlePause(const audio::Token &token) -> std::unique_ptr<AudioResponseMessage>;
    auto HandlePause(std::optional<audio::AudioMux::Input *> input) -> std::unique_ptr<AudioResponseMessage>;
    auto HandleResume(const audio::Token &token) -> std::unique_ptr<AudioResponseMessage>;
    void HandleEOF(const audio::Token &token);
    auto HandleKeyPressed(const int step) -> sys::MessagePointer;
    void MuteCurrentOperation();
    void VibrationUpdate(const audio::PlaybackType &type               = audio::PlaybackType::None,
                         std::optional<audio::AudioMux::Input *> input = std::nullopt);
    void DisableVibration(std::optional<audio::AudioMux::Input *> input = std::nullopt);
    void EnableOneShotVibration();
    void EnableContinuousVibration(std::optional<audio::AudioMux::Input *> input = std::nullopt);

    auto GetVibrationType(const audio::PlaybackType &type) -> VibrationType;

    auto IsVibrationEnabled(const audio::PlaybackType &type) -> bool;
    auto IsOperationEnabled(const audio::PlaybackType &plType, const audio::Operation::Type &opType) -> bool;
    std::string GetSound(const audio::PlaybackType &plType);
    constexpr auto IsResumable(const audio::PlaybackType &type) const -> bool;
    constexpr auto ShouldLoop(const std::optional<audio::PlaybackType> &type) const -> bool;
    auto IsBusy() -> bool;
    auto IsSystemSound(const audio::PlaybackType &type) -> bool;
    audio::PlaybackType generatePlayback(const audio::PlaybackType &type,
                                         const audio::Setting &setting = audio::Setting::Volume);
    audio::Profile::Type generateProfile(const audio::Profile::Type &profile, const audio::PlaybackType &playbackType);

    //! Setter for settings
    //! \param setting Setting to be controlled
    //! \param value New value of setting
    //! \param profileType Audio profile to be controlled
    //! \param playbackType Playback type to be controlled
    //! \note when profileType and playbackType are not set currently active sound will be controlled
    void setSetting(const audio::Setting &setting,
                    const std::string &value,
                    const audio::Profile::Type &profileType = audio::Profile::Type::Idle,
                    const audio::PlaybackType &playbackType = audio::PlaybackType::None);

    [[nodiscard]] std::string getSetting(const audio::Setting &setting,
                                         const audio::Profile::Type &profileType,
                                         const audio::PlaybackType &playbackType);

    const audio::Context getCurrentContext();
    void settingsChanged(const std::string &name, std::string value);
    void onVolumeChanged(audio::Volume volume, audio::VolumeChangeRequestSource source);
    auto handleA2DPVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer;
    auto handleHSPVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer;
    auto handleHFPVolumeChangedOnBluetoothDevice(sys::Message *msgl) -> sys::MessagePointer;
    auto handleMultimediaAudioPause() -> sys::MessagePointer;
    auto handleMultimediaAudioStart() -> sys::MessagePointer;
};

namespace sys
{
    template <>
    struct ManifestTraits<ServiceAudio>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::audio;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
