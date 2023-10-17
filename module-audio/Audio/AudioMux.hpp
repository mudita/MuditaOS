// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioCommon.hpp"
#include "Audio.hpp"

namespace audio
{
    /**
     * Container for audio::Audio instances, handles the logic for choosing right input, but does not perform
     * operations on those inputs
     */
    class AudioMux
    {
      public:
        enum class VibrationStatus
        {
            On,
            Off
        };

        struct Input
        {
            Input(std::unique_ptr<audio::Audio> a, audio::Token handle) : audio(std::move(a)), token(handle)
            {}
            std::unique_ptr<audio::Audio> audio;
            audio::Token token;
            VibrationStatus vibrationOn = VibrationStatus::Off;

            void EnableVibration()
            {
                vibrationOn = VibrationStatus::On;
            }
            void DisableVibration()
            {
                vibrationOn = VibrationStatus::Off;
            }
            auto GetVibrationStatus() const -> VibrationStatus
            {
                return vibrationOn;
            };
        };
        /**
         * Constructs class with fixed number of managed inputs
         * @param callback Callback for async requests to audio service from audio module
         * @param audioInputsCount Number of inputs managed and internal audio::Audio() classes created
         */
        AudioMux(AudioServiceMessage::Callback callback, size_t audioInputsCount = 1);
        /**
         * Constructs mux managing externally allocated instances of Input
         * @param extAudioInputs Instances of Input to be managed
         */
        AudioMux(std::vector<Input> &extAudioInputs) : audioInputs(extAudioInputs)
        {}
        /**
         * Gets input related to given token
         * @param token Token to compare
         * @return nullopt if input not found
         */
        auto GetInput(const Token &token) -> std::optional<Input *>;
        /**
         * Gets first input which is in given state
         * @param states State to compare
         * @return nullopt if input not found
         */
        auto GetInput(const std::vector<Audio::State> &states) -> std::optional<Input *>;
        /**
         * Gets available input respecting priority
         * @param playbackType Playback type to compare to compare
         * @return Idle input if found, otherwise one of inputs that can be overridden.
         * nullopt if none of the inputs is not available (higher priority operation is active)
         */
        auto GetAvailableInput(const audio::PlaybackType &playbackType = audio::PlaybackType::None)
            -> std::optional<Input *>;
        /**
         * Gets first Idle input
         * @return nullopt if input not found
         */
        auto GetIdleInput() -> std::optional<Input *>;
        /**
         * Gets input, if possible routing one, otherwise first playback
         * @return nullopt if input not found
         */
        auto GetActiveInput() -> std::optional<Input *>;
        /**
         * Gets input for routing
         * @param force If set upon failure returns one of busy inputs
         * @return nullopt if input not found
         */
        auto GetRoutingInput(bool force = false) -> std::optional<Input *>;
        /**
         * Gets input for playback
         * @param playbackType Playback type to compare
         * @return nullopt if input not found
         */
        auto GetPlaybackInput(const audio::PlaybackType &playbackType) -> std::optional<AudioMux::Input *>;

        auto GetAllInputs() -> std::vector<Input> &
        {
            return audioInputs;
        };
        auto ResetInput(std::optional<AudioMux::Input *> input = std::nullopt) -> const Token;

      private:
        auto GetPlaybackPriority(const std::optional<audio::PlaybackType> type) -> uint8_t;
        constexpr auto IsMergable(const audio::PlaybackType &type) const -> bool;

        audio::Token refToken;
        std::vector<Input> audioInputsInternal;
        std::vector<Input> &audioInputs;
    };

} // namespace audio
