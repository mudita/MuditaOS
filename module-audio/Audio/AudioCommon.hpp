#pragma once

#include <map>

#include "Profiles/Profile.hpp"
#include "bsp_audio.hpp"

namespace audio
{
    class AudioMux;
}

namespace audio
{
    constexpr Volume defaultVolumeStep = 1;
    constexpr Gain defaultGainStep     = 10;
    constexpr Volume defaultVolume     = 5;
    constexpr Gain defaultGain         = 5;

    constexpr Volume maxVolume = 10;
    constexpr Volume minVolume = 0;

    constexpr Gain maxGain = 100;
    constexpr Gain minGain = 0;

    constexpr uint32_t audioOperationTimeout = 1000;

    enum class Setting
    {
        Volume,
        Gain,
        EnableVibration,
        EnableSound
    };

    enum class PlaybackType
    {
        None,
        Multimedia,
        Notifications,
        KeypadSound,
        CallRingtone,
        TextMessageRingtone,
        Last = TextMessageRingtone,
    };

    // enum class Key
    // {
    //     None,
    //     VolumeUp,
    //     VolumeDown
    // }

    const static std::map<PlaybackType, uint8_t> PlaybackTypePriority = {
        {PlaybackType::CallRingtone, 2},
        {PlaybackType::TextMessageRingtone, 3},
        {PlaybackType::Notifications, 3},
        {PlaybackType::Multimedia, 4},
        {PlaybackType::KeypadSound, 5},
        {PlaybackType::None, static_cast<uint8_t>(PlaybackType::Last)},
    };

    [[nodiscard]] const std::string str(const PlaybackType &playbackType) noexcept;

    [[nodiscard]] const std::string str(const Setting &setting) noexcept;

    [[nodiscard]] const std::string str(const Setting &setting,
                                        const PlaybackType &playbackType,
                                        const Profile::Type &profileType);

    [[nodiscard]] const std::string str(const Setting &setting,
                                        const PlaybackType &playbackType,
                                        const bool headphonesInserted);

    [[nodiscard]] const std::string str(const Setting &setting, const PlaybackType &playbackType);

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
        Failed
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

    class Handle
    {
      public:
        Handle(const RetCode &retCode = RetCode::Failed, const Token &token = Token())
            : lastRetCode(retCode), token(token)
        {}
        auto GetLastRetCode() -> RetCode
        {
            return lastRetCode;
        }
        auto GetToken() const -> const Token &
        {
            return token;
        }

      private:
        RetCode lastRetCode;
        Token token;
    };

    enum class PlaybackEventType
    {
        Empty,
        EndOfFile,
        FileSystemNoSpace
    };

    struct PlaybackEvent
    {
        PlaybackEventType event = PlaybackEventType::Empty;
        audio::Token token      = audio::Token::MakeBadToken();
    };

    typedef std::function<int32_t(PlaybackEvent e)> AsyncCallback;
    typedef std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> DbCallback;

    RetCode GetDeviceError(bsp::AudioDevice::RetCode retCode);
    const std::string str(RetCode retcode);
    [[nodiscard]] auto GetVolumeText(const audio::Volume &volume) -> const std::string;
} // namespace audio

namespace audio::notifications
{
    const std::vector<audio::PlaybackType> typesToMute = {audio::PlaybackType::Notifications,
                                                          audio::PlaybackType::CallRingtone,
                                                          audio::PlaybackType::TextMessageRingtone};
} // namespace audio::notifications
