// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "BaseInterface.hpp" // for Interface, Inter...

#include <Audio/Audio.hpp>
#include <Audio/AudioMux.hpp>  // for AudioMux, AudioM...
#include <Service/Service.hpp> // for Service
#include <queries/settings/QuerySettingsAddOrIgnore_v2.hpp>
#include <queries/settings/QuerySettingsGet_v2.hpp> // for SettingsQuery
#include <queries/settings/QuerySettingsUpdate_v2.hpp>
#include <MessageType.hpp>
#include <Utils.hpp>
#include <assert.h> // for assert
#include <stdint.h> // for uint32_t, int32_t

#include "Audio/AudioCommon.hpp"                // for PlaybackType
#include "Audio/Operation/Operation.hpp"        // for Operation, Opera...
#include "Audio/Profiles/Profile.hpp"           // for Profile, Profile...
#include "Common/Query.hpp"                     // for Query, QueryResult
#include "Service/Common.hpp"                   // for ReturnCodes, Ret...
#include "Service/Message.hpp"                  // for Message_t, DataM...
#include "messages/AudioMessage.hpp"            // for AudioNotificatio...
#include <service-db/api/DBServiceAPI.hpp>      // for DBServiceAPI
#include <service-db/messages/QueryMessage.hpp> // for QueryResponse
#include <functional>
#include <memory>   // for unique_ptr, shar...
#include <optional> // for optional, nullopt
#include <string>   // for string
#include <utility>  // for pair, tuple_elem...
#include <vector>   // for vector

class ServiceAudio : public sys::Service
{
  public:
    ServiceAudio();

    ~ServiceAudio();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

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

    auto IsVibrationMotorOn()
    {
        return vibrationMotorStatus == audio::AudioMux::VibrationStatus::On;
    }

    auto AsyncCallback(audio::PlaybackEvent e) -> int32_t;
    auto DbCallback(const std::string &path, const uint32_t &defaultValue) -> uint32_t;

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

    void addOrIgnoreEntry(const std::string &profilePath, const std::string &defaultValue);

    template <typename T>[[nodiscard]] T fetchAudioSettingFromDb(const std::string &profilePath, const T &defaultValue)
    {
        auto [code, msg] =
            DBServiceAPI::GetQueryWithReply(this,
                                            db::Interface::Name::Settings_v2,
                                            std::make_unique<db::query::settings::SettingsQuery>(profilePath),
                                            audio::audioOperationTimeout);

        if (code == sys::ReturnCodes::Success && msg != nullptr) {
            auto queryResponse = dynamic_cast<db::QueryResponse *>(msg.get());
            assert(queryResponse != nullptr);

            auto settingsResultResponse = queryResponse->getResult();
            auto settingsResult = dynamic_cast<db::query::settings::SettingsResult *>(settingsResultResponse.get());
            assert(settingsResult != nullptr);

            return settingsResult->getResult().getValue<T>({});
        }
        return defaultValue;
    }
    void updateDbValue(const std::string &path, const std::string &value);

    void setSetting(const audio::Setting &setting,
                    const std::string &value,
                    const audio::Profile::Type &profileType,
                    const audio::PlaybackType &playbackType);
    [[nodiscard]] std::string getSetting(const audio::Setting &setting,
                                         const audio::Profile::Type &profileType,
                                         const audio::PlaybackType &playbackType);

    const std::pair<audio::Profile::Type, audio::PlaybackType> getCurrentContext();
};
