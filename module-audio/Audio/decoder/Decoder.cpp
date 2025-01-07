// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <cstdio>
#include <Utils.hpp>
#include "Decoder.hpp"
#include "DecoderMP3.hpp"
#include "DecoderFLAC.hpp"
#include "DecoderWAV.hpp"

#include "tag.h"
#include <tags_fetcher/TagsFetcher.hpp>

namespace audio
{
    Decoder::Decoder(const std::string &path) : filePath(path)
    {
        fd = std::fopen(path.c_str(), "rb");
        if (fd == nullptr) {
            return;
        }

        constexpr std::size_t streamBufferSize = 16 * 1024;
        streamBuffer                      = std::make_unique<char[]>(streamBufferSize);
        setvbuf(fd, streamBuffer.get(), _IOFBF, streamBufferSize);

        std::fseek(fd, 0, SEEK_END);
        fileSize = std::ftell(fd);
        std::rewind(fd);

        tags = fetchTags();
    }

    Decoder::~Decoder()
    {
        if (audioWorker) {
            audioWorker->close();
        }

        if (fd != nullptr) {
            std::fclose(fd);
        }
    }

    auto Decoder::fetchTags() -> std::unique_ptr<tags::fetcher::Tags>
    {
        return std::make_unique<tags::fetcher::Tags>(tags::fetcher::fetchTags(filePath));
    }

    auto Decoder::Create(const std::string &filePath) -> std::unique_ptr<Decoder>
    {
        const auto extension          = std::filesystem::path(filePath).extension();
        const auto extensionLowercase = utils::stringToLowercase(extension);

        std::unique_ptr<Decoder> dec;

        if (extensionLowercase == ".wav") {
            dec = std::make_unique<DecoderWAV>(filePath);
        }
        else if (extensionLowercase == ".mp3") {
            dec = std::make_unique<DecoderMP3>(filePath);
        }
        else if (extensionLowercase == ".flac") {
            dec = std::make_unique<DecoderFLAC>(filePath);
        }
        else {
            return nullptr;
        }

        if (!dec->isInitialized) {
            return nullptr;
        }

        return dec;
    }

    auto Decoder::startDecodingWorker(const DecoderWorker::EndOfFileCallback &endOfFileCallback,
                                      const DecoderWorker::FileDeletedCallback &fileDeletedCallback) -> void
    {
        assert(_stream != nullptr);

        if (audioWorker == nullptr) {
            const auto channelMode = (tags->num_channel == 1) ? DecoderWorker::ChannelMode::ForceStereo
                                                              : DecoderWorker::ChannelMode::NoConversion;

            audioWorker =
                std::make_unique<DecoderWorker>(_stream, this, endOfFileCallback, fileDeletedCallback, channelMode);
            audioWorker->init();
            audioWorker->run();
        }
        else {
            LOG_DEBUG("AudioWorker already running.");
        }
    }

    auto Decoder::stopDecodingWorker() -> void
    {
        if (audioWorker) {
            audioWorker->close();
        }
        audioWorker = nullptr;
    }

    auto Decoder::onDataReceive() -> void
    {
        audioWorker->enablePlayback();
    }

    auto Decoder::enableInput() -> void
    {
        audioWorker->enablePlayback();
    }

    auto Decoder::disableInput() -> void
    {
        audioWorker->disablePlayback();
    }

    auto Decoder::getSourceFormat() -> AudioFormat
    {
        auto bitWidth = getBitWidth();
        // this is a decoder mono to stereo hack, will be removed when proper
        // transcoding implementation is added
        auto channels = (tags->num_channel == 1) ? 2U : tags->num_channel;

        return AudioFormat{tags->sample_rate, bitWidth, channels};
    }

    auto Decoder::getSupportedFormats() -> std::vector<AudioFormat>
    {
        return std::vector<AudioFormat>{getSourceFormat()};
    }

    auto Decoder::getTraits() const -> Endpoint::Traits
    {
        return Endpoint::Traits{};
    }
} // namespace audio
