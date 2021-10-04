// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioDevice.hpp"
#include "Profiles/Profile.hpp"

#include <Service/Message.hpp>
#include <Utils.hpp>

#include <bitset>
#include <map>
#include <memory>
#include <utility>

namespace audio
{
    class AudioMux;
}

namespace audio
{
    inline constexpr Volume defaultVolumeStep = 1;
    inline constexpr Gain defaultGainStep     = 10;
    inline constexpr Volume defaultVolume     = 5;
    inline constexpr Gain defaultGain         = 5;

    inline constexpr Volume maxVolume = 10;
    inline constexpr Volume minVolume = 0;

    inline constexpr Gain maxGain = 100;
    inline constexpr Gain minGain = 0;

    inline constexpr auto audioOperationTimeout = 1000U;

    inline constexpr auto audioDbPrefix   = "audio";
    inline constexpr auto systemDbPrefix  = "system";
    inline constexpr auto dbPathSeparator = '/';

    enum class Setting
    {
        Volume,
        Gain,
        EnableVibration,
        EnableSound,
        Sound,
        IsSystemSound
    };

    enum class SettingState : bool
    {
        Enabled,
        Disabled
    };

    enum class PlaybackType
    {
        None,
        Multimedia,
        Notifications,
        System = Notifications,
        KeypadSound,
        CallRingtone,
        TextMessageRingtone,
        Meditation,
        Alarm,
        Last = Alarm,
    };

    /// Used to describe audio operations
    using Context = std::pair<Profile::Type, PlaybackType>;

    struct DbPathElement
    {
        Setting setting;
        PlaybackType playbackType;
        Profile::Type profileType;
    };

    [[nodiscard]] const std::string str(const PlaybackType &playbackType) noexcept;

    [[nodiscard]] const std::string str(const Setting &setting) noexcept;

    [[nodiscard]] const std::string dbPath(const DbPathElement &element);

    [[nodiscard]] const std::string dbPath(const Setting &setting,
                                           const PlaybackType &playbackType,
                                           const Profile::Type &profileType);

    enum class EventType
    {
        // HW state change notifications
        JackState,               //!< jack input plugged / unplugged event
        BlutoothHSPDeviceState,  //!< BT device connected / disconnected event (Headset Profile)
        BlutoothHFPDeviceState,  //!< BT device connected / disconnected event (Headset Profile)
        BlutoothA2DPDeviceState, //!< BT device connected / disconnected event (Advanced Audio Distribution Profile)

        // call control
        CallMute,
        CallUnmute,
        CallLoudspeakerOn,
        CallLoudspeakerOff,
    };

    constexpr auto hwStateUpdateMaxEvent = magic_enum::enum_index(EventType::BlutoothA2DPDeviceState);

    class Event
    {
      public:
        enum class DeviceState
        {
            Connected,
            Disconnected
        };

        explicit Event(EventType eType, DeviceState deviceState = DeviceState::Connected)
            : eventType(eType), deviceState(deviceState)
        {}

        virtual ~Event() = default;

        EventType getType() const noexcept
        {
            return eventType;
        }

        DeviceState getDeviceState() const noexcept
        {
            return deviceState;
        }

      private:
        const EventType eventType;
        const DeviceState deviceState;
    };

    class AudioSinkState
    {
      public:
        void UpdateState(std::shared_ptr<Event> stateChangeEvent)
        {
            auto hwUpdateEventIdx = magic_enum::enum_integer(stateChangeEvent->getType());
            if (hwUpdateEventIdx <= hwStateUpdateMaxEvent) {
                audioSinkState.set(hwUpdateEventIdx,
                                   stateChangeEvent->getDeviceState() == Event::DeviceState::Connected ? true : false);
            }
        }

        std::vector<std::shared_ptr<Event>> getUpdateEvents() const
        {
            std::vector<std::shared_ptr<Event>> updateEvents;
            for (size_t i = 0; i <= hwStateUpdateMaxEvent; i++) {
                auto isConnected =
                    audioSinkState.test(i) ? Event::DeviceState::Connected : Event::DeviceState::Disconnected;
                auto updateEvt = magic_enum::enum_cast<EventType>(i);
                updateEvents.emplace_back(std::make_unique<Event>(updateEvt.value(), isConnected));
            }
            return updateEvents;
        }

        bool isConnected(EventType deviceUpdateEvent) const
        {
            return audioSinkState.test(magic_enum::enum_integer(deviceUpdateEvent));
        }

        void setConnected(EventType deviceUpdateEvent, bool isConnected)
        {
            audioSinkState.set(magic_enum::enum_integer(deviceUpdateEvent), isConnected);
        }

      private:
        std::bitset<magic_enum::enum_count<EventType>()> audioSinkState;
    };

    enum class RetCode
    {
        Success = 0,
        InvokedInIncorrectState,
        UnsupportedProfile,
        UnsupportedEvent,
        InvalidFormat,
        OperationCreateFailed,
        FileDoesntExist,
        FailedToAllocateMemory,
        OperationNotSet,
        ProfileNotSet,
        DeviceFailure,
        TokenNotFound,
        Ignored,
        Failed
    };

    struct AudioInitException : public std::runtime_error
    {
      protected:
        audio::RetCode errorCode = audio::RetCode::Failed;

      public:
        AudioInitException(const char *message, audio::RetCode errorCode) : runtime_error(message)
        {}

        audio::RetCode getErrorCode() const noexcept
        {
            return errorCode;
        }
    };

    class Token
    {
        using TokenType = int16_t;

      public:
        explicit Token(TokenType initValue = tokenUninitialized) : t(initValue)
        {}

        bool operator==(const Token &other) const noexcept
        {
            return other.t == t;
        }

        bool operator!=(const Token &other) const noexcept
        {
            return !(other.t == t);
        }

        /**
         * Valid token is one connected with existing sequence of operations
         * @return True if valid, false otherwise
         */
        bool IsValid() const
        {
            return t > tokenUninitialized;
        }
        /**
         * Bad token cannot be used anymore
         * @return True if token is flagged bad
         */
        bool IsBad() const
        {
            return t == tokenBad;
        }
        /**
         * Uninitialized token can be used but it is not connected to any sequence of operations
         * @return True if token is flagged uninitialized
         */
        bool IsUninitialized() const
        {
            return t == tokenUninitialized;
        }
        /**
         * Helper - returns bad Token
         * @return Unusable bad Token
         */
        static inline Token MakeBadToken()
        {
            return Token(tokenBad);
        }

      private:
        static constexpr auto maxToken = std::numeric_limits<TokenType>::max();
        Token IncrementToken()
        {
            t = (t == maxToken) ? 0 : t + 1;
            return *this;
        }

        constexpr static TokenType tokenUninitialized = -1;
        constexpr static TokenType tokenBad           = -2;

        TokenType t;
        friend class ::audio::AudioMux;
    };

    RetCode GetDeviceError(AudioDevice::RetCode retCode);
    const std::string str(RetCode retcode);
    [[nodiscard]] auto GetVolumeText(const audio::Volume &volume) -> std::string;
} // namespace audio

namespace AudioServiceMessage
{
    class EndOfFile : public sys::DataMessage
    {
      public:
        explicit EndOfFile(audio::Token &token) : token(token)
        {}
        const audio::Token &GetToken() const
        {
            return token;
        }

      private:
        audio::Token token = audio::Token::MakeBadToken();
    };

    class FileSystemNoSpace : public sys::DataMessage
    {
      public:
        explicit FileSystemNoSpace(audio::Token &token) : token(token)
        {}
        const audio::Token &GetToken() const
        {
            return token;
        }

      private:
        audio::Token token = audio::Token::MakeBadToken();
    };

    class DbRequest : public sys::DataMessage
    {
      public:
        explicit DbRequest(const audio::Setting &setting,
                           const audio::PlaybackType &playback,
                           const audio::Profile::Type &profile)
            : setting(setting), profile(profile), playback(playback)
        {}

        const audio::Setting setting;
        const audio::Profile::Type profile;
        const audio::PlaybackType playback;
    };

    class AudioDeviceCreated : public sys::DataMessage
    {
      public:
        explicit AudioDeviceCreated(std::shared_ptr<audio::AudioDevice> device, audio::AudioDevice::Type type)
            : device(std::move(device)), type(type)
        {}

        auto getDevice() const noexcept -> std::shared_ptr<audio::AudioDevice>
        {
            return device;
        }

        auto getDeviceType() const noexcept -> audio::AudioDevice::Type
        {
            return type;
        }

      private:
        std::shared_ptr<audio::AudioDevice> device;
        audio::AudioDevice::Type type;
    };

    using Callback = std::function<std::optional<std::string>(const sys::Message *msg)>;
} // namespace AudioServiceMessage
