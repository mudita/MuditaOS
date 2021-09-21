// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Audio/AudioCommon.hpp"
#include "Audio/Endpoint.hpp"
#include "Audio/Stream.hpp"
#include "DecoderWorker.hpp"

#include <log/log.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <cstring>
#include <cstdint>
#include <tags_fetcher/TagsFetcher.hpp>

namespace audio
{
    namespace channel
    {
        constexpr inline auto monoSound   = 1;
        constexpr inline auto stereoSound = 2;
    } // namespace channel

    class Decoder : public Source
    {

      public:
        Decoder(const char *fileName);

        virtual ~Decoder();

        virtual uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) = 0;

        // Range 0 - 1
        virtual void setPosition(float pos) = 0;

        uint32_t getSampleRate()
        {
            return sampleRate;
        }

        uint32_t getChannelNumber()
        {
            return chanNumber;
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

        void startDecodingWorker(DecoderWorker::EndOfFileCallback endOfFileCallback);
        void stopDecodingWorker();

        // Factory method
        static std::unique_ptr<Decoder> Create(const char *file);

      protected:
        virtual auto getBitWidth() -> unsigned int = 0;
        virtual std::unique_ptr<tags::fetcher::Tags> fetchTags();

        void convertmono2stereo(int16_t *pcm, uint32_t samplecount);

        static constexpr auto workerBufferSize        = 1024 * 8;
        static constexpr Endpoint::Traits decoderCaps = {.usesDMA = false};

        uint32_t sampleRate = 0;
        uint32_t chanNumber = 0;
        float position      = 0;
        std::FILE *fd       = nullptr;
        uint32_t fileSize   = 0;
        std::string filePath;

        // Worker buffer used for converting mono stream to stereo
        std::unique_ptr<int16_t[]> workerBuffer;
        std::unique_ptr<tags::fetcher::Tags> tags;
        bool isInitialized = false;

        // decoding worker
        std::unique_ptr<DecoderWorker> audioWorker;
        DecoderWorker::EndOfFileCallback _endOfFileCallback;
    };

} // namespace audio
