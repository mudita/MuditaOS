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
        struct Input
        {
            Input(audio::Audio &&a, audio::Token handle) : audio(std::move(a)), token(handle)
            {}
            audio::Audio audio;
            audio::Token token;
        };
        /**
         * Constructs class with fixed number of managed inputs
         * @param asyncClbk Callback for async control events from of managed audio::Audio() instances
         * @param dbClbk Callback for async DB change events from of managed audio::Audio() instances
         * @param audioInputsCount Number of inputs managed and internal audio::Audio() classes created
         */
        AudioMux(audio::AsyncCallback asyncClbk, audio::DbCallback dbClbk, size_t audioInputsCount = 1);
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
        auto GetInput(const audio::PlaybackType &playbackType = audio::PlaybackType::None) -> std::optional<Input *>;
        /**
         * Gets first Idle input
         * @return nullopt if input not found
         */
        auto GetIdleInput() -> std::optional<Input *>;
        /**
         * Gets first Active input
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
         * Gets input for recording
         * @return nullopt if input not found
         */
        auto GetRecordingInput() -> std::optional<Input *>;
        /**
         * Gets input for playback
         * @param token Token to compare
         * @param playbackType Playback type to compare
         * @return nullopt if input not found
         */
        auto GetPlaybackInput(const Token &token, const audio::PlaybackType &playbackType)
            -> std::optional<AudioMux::Input *>;

        auto GetAllInputs() -> std::vector<Input> &
        {
            return audioInputs;
        };
        auto IncrementToken(std::optional<AudioMux::Input *> input) -> Token;

      private:
        auto GetPlaybackPriority(const audio::PlaybackType &type) -> uint8_t;
        auto IsMergable(const audio::PlaybackType &type) -> uint8_t;
        std::vector<Input> audioInputs;
        audio::Token refToken;
    };

} // namespace audio
