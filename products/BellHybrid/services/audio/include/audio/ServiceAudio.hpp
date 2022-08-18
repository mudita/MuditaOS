// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioMessage.hpp"
#include <Audio/Audio.hpp>
#include <Audio/AudioMux.hpp>
#include <MessageType.hpp>
#include <service-db/DBServiceName.hpp>
#include <Service/Service.hpp>
#include <SystemManager/CpuSentinel.hpp>

#include <functional>

namespace settings
{
    class Settings;
}

namespace service
{
    class Audio : public sys::Service
    {
      public:
        Audio();

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) final;
        sys::ReturnCodes InitHandler() final;
        sys::ReturnCodes DeinitHandler() final;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) final;
        void ProcessCloseReason(sys::CloseReason closeReason) final;

      private:
        enum class StopReason
        {
            Eof,
            Other
        };

        auto handleStart(audio::Operation::Type opType,
                         const std::string &fileName             = {},
                         const audio::PlaybackType &playbackType = audio::PlaybackType::None)
            -> std::unique_ptr<AudioResponseMessage>;
        auto handleStop(const std::vector<audio::PlaybackType> &stopTypes, const audio::Token &token)
            -> std::unique_ptr<AudioResponseMessage>;

        auto handleSetVolume(const audio::PlaybackType &playbackType, const std::string &value)
            -> std::unique_ptr<AudioResponseMessage>;
        auto handleGetVolume(const audio::PlaybackType &playbackType) -> std::unique_ptr<AudioResponseMessage>;

        auto handlePause() -> std::unique_ptr<AudioResponseMessage>;
        auto handleResume() -> std::unique_ptr<AudioResponseMessage>;

        void handleEOF(const audio::Token &token);

        auto AudioServicesCallback(const sys::Message *msg) -> std::optional<std::string>;

        auto stopInput(audio::AudioMux::Input *input, StopReason stopReason = StopReason::Other) -> audio::RetCode;

        constexpr auto shouldLoop(const std::optional<audio::PlaybackType> &type) const -> bool;
        constexpr auto isResumable(audio::PlaybackType type) const -> bool;
        auto isBusy() const -> bool;

        void manageCpuSentinel();
        void initializeDatabase();

        mutable audio::AudioMux audioMux;
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
        std::unique_ptr<settings::Settings> settingsProvider;
    };

} // namespace service

namespace sys
{
    template <>
    struct ManifestTraits<service::Audio>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::audioServiceName;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
