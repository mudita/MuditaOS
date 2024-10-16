// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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

        virtual std::int32_t decode(std::uint32_t samplesToRead, std::int16_t *pcmData) = 0;

        // Range 0 - 1
        virtual void setPosition(float pos) = 0;

        std::uint32_t getSampleRate()
        {
            return sampleRate;
        }

        std::uint32_t getChannelCount()
        {
            return channelCount;
        }

        float getCurrentPosition()
        {
            return position;
        }

        void onDataReceive() override;
        void enableInput() override;
        void disableInput() override;

        auto getSourceFormat() -> AudioFormat override;
        auto getSupportedFormats() -> std::vector<AudioFormat> override;

        auto getTraits() const -> Endpoint::Traits override;

        void startDecodingWorker(const DecoderWorker::EndOfFileCallback &endOfFileCallback,
                                 const DecoderWorker::FileDeletedCallback &fileDeletedCallback);
        void stopDecodingWorker();

        // Factory method
        static std::unique_ptr<Decoder> Create(const std::string &path);

      protected:
        virtual auto getBitWidth() -> unsigned int
        {
            return bitsPerSample;
        }

        virtual std::unique_ptr<tags::fetcher::Tags> fetchTags();

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
