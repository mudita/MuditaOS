// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

    std::unique_ptr<tags::fetcher::Tags> Decoder::fetchTags()
    {
        return std::make_unique<tags::fetcher::Tags>(tags::fetcher::fetchTags(filePath));
    }

    std::unique_ptr<Decoder> Decoder::Create(const std::string &filePath)
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

    void Decoder::startDecodingWorker(const DecoderWorker::EndOfFileCallback &endOfFileCallback,
                                      const DecoderWorker::FileDeletedCallback &fileDeletedCallback)
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

    void Decoder::stopDecodingWorker()
    {
        if (audioWorker) {
            audioWorker->close();
        }
        audioWorker = nullptr;
    }

    void Decoder::onDataReceive()
    {
        audioWorker->enablePlayback();
    }

    void Decoder::enableInput()
    {
        audioWorker->enablePlayback();
    }

    void Decoder::disableInput()
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
