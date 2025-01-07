// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Audio/AudioCommon.hpp"
#include "Audio/Endpoint.hpp"
#include "DecoderWorker.hpp"

#include <memory>
#include <vector>

#include <tags_fetcher/TagsFetcher.hpp>

namespace audio
{
    namespace channel
    {
        inline constexpr auto monoSound   = 1;
        inline constexpr auto stereoSound = 2;
    } // namespace channel

    class Decoder : public Source
    {
      public:
        static constexpr auto fileDeletedRetCode = -1;

        explicit Decoder(const std::string &path);
        virtual ~Decoder();

        virtual auto decode(std::uint32_t samplesToRead, std::int16_t *pcmData) -> std::int32_t = 0;

        // Range 0 - 1
        virtual auto setPosition(float pos) -> void = 0;

        // Rewind to first audio sample
        virtual auto rewind() -> void = 0;

        [[nodiscard]] auto getSampleRate() const noexcept -> std::uint32_t
        {
            return sampleRate;
        }

        [[nodiscard]] auto getChannelCount() const noexcept -> std::uint32_t
        {
            return channelCount;
        }

        [[nodiscard]] auto getCurrentPosition() const noexcept -> float
        {
            return position;
        }

        auto onDataReceive() -> void override;
        auto enableInput() -> void override;
        auto disableInput() -> void override;

        auto getSourceFormat() -> AudioFormat override;
        auto getSupportedFormats() -> std::vector<AudioFormat> override;

        auto getTraits() const -> Endpoint::Traits override;

        auto startDecodingWorker(const DecoderWorker::EndOfFileCallback &endOfFileCallback,
                                 const DecoderWorker::FileDeletedCallback &fileDeletedCallback) -> void;
        auto stopDecodingWorker() -> void;

        // Factory method
        static auto Create(const std::string &path) -> std::unique_ptr<Decoder>;

      protected:
        [[nodiscard]] virtual auto getBitWidth() -> unsigned int
        {
            return bitsPerSample;
        }

        virtual auto fetchTags() -> std::unique_ptr<tags::fetcher::Tags>;

        static constexpr Endpoint::Traits decoderCaps = {.usesDMA = false};

        std::uint32_t sampleRate = 0;
        std::uint32_t channelCount = 0;
        std::uint32_t bitsPerSample;
        float position = 0;
        std::FILE *fd  = nullptr;
        std::unique_ptr<char[]> streamBuffer;
        std::uint32_t fileSize = 0;
        std::string filePath;

        std::unique_ptr<tags::fetcher::Tags> tags;
        bool isInitialized = false;

        // Decoding worker
        std::unique_ptr<DecoderWorker> audioWorker;
    };
} // namespace audio
