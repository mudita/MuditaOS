// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstdio>
#include <Utils.hpp>
#include "Decoder.hpp"
#include "decoderMP3.hpp"
#include "decoderFLAC.hpp"
#include "decoderWAV.hpp"

#include "fileref.h"
#include "tag.h"
#include "tfilestream.h"

namespace audio
{
    Decoder::Decoder(const char *fileName)
        : filePath(fileName), workerBuffer(std::make_unique<int16_t[]>(workerBufferSize)), tag(std::make_unique<Tags>())
    {

        fd = std::fopen(fileName, "r");
        if (fd == NULL) {
            return;
        }

        std::fseek(fd, 0, SEEK_END);
        fileSize = std::ftell(fd);
        std::rewind(fd);
    }

    Decoder::~Decoder()
    {
        if (audioWorker) {
            audioWorker->close();
        }

        if (fd) {
            std::fclose(fd);
        }
    }

    std::unique_ptr<Tags> Decoder::fetchTags()
    {
        if (fd) {
            auto inPos = std::ftell(fd);
            std::rewind(fd);
            TagLib::FileStream fileStream(fd);
            TagLib::FileRef tagReader(&fileStream);
            if (!tagReader.isNull() && tagReader.tag()) {
                TagLib::Tag *tags                   = tagReader.tag();
                TagLib::AudioProperties *properties = tagReader.audioProperties();

                tag->title  = tags->title().to8Bit();
                tag->artist = tags->artist().to8Bit();
                tag->album  = tags->album().to8Bit();
                tag->genre  = tags->genre().to8Bit();
                tag->year   = std::to_string(tags->year());

                tag->total_duration_s = properties->length();
                tag->duration_min     = tag->total_duration_s / utils::secondsInMinute;
                tag->duration_hour    = tag->duration_min / utils::secondsInMinute;
                tag->duration_sec     = tag->total_duration_s % utils::secondsInMinute;
                tag->sample_rate      = properties->sampleRate();
                tag->num_channel      = properties->channels();
                tag->bitrate          = properties->bitrate();
            }
            std::rewind(fd);
            fetchTagsSpecific();
            std::fseek(fd, inPos, SEEK_SET);
        }

        tag->filePath.append(filePath);
        // If title tag empty fill it with raw file name
        if (tag->title.size() == 0) {
            if (const auto pos = filePath.rfind("/"); pos == std::string::npos) {
                tag->title.append(filePath);
            }
            else {
                tag->title.append(&filePath[pos + 1]);
            }
        }
        return std::make_unique<Tags>(*tag);
    }

    std::unique_ptr<Decoder> Decoder::Create(const char *file)
    {
        std::unique_ptr<Decoder> dec;
        if ((strstr(file, ".wav") != NULL) || (strstr(file, ".WAV") != NULL)) {
            dec = std::make_unique<decoderWAV>(file);
        }
        else if ((strstr(file, ".mp3") != NULL) || (strstr(file, ".MP3") != NULL)) {
            dec = std::make_unique<decoderMP3>(file);
        }
        else if ((strstr(file, ".flac") != NULL) || (strstr(file, ".FLAC") != NULL)) {
            dec = std::make_unique<decoderFLAC>(file);
        }
        else {
            return nullptr;
        }

        if (!dec->isInitialized) {
            return nullptr;
        }
        else {
            return dec;
        }
    }

    void Decoder::convertmono2stereo(int16_t *pcm, uint32_t samplecount)
    {
        uint32_t i = 0, j = 0;

        memset(workerBuffer.get(), 0, workerBufferSize * sizeof(int16_t));

        for (; j < samplecount; j++) {
            workerBuffer[i++] = pcm[j];
            workerBuffer[i++] = pcm[j];
        }

        memcpy(pcm, &workerBuffer[0], samplecount * 2 * sizeof(int16_t));
    }

    void Decoder::startDecodingWorker(DecoderWorker::EndOfFileCallback endOfFileCallback)
    {
        assert(_stream != nullptr);
        if (!audioWorker) {
            audioWorker =
                std::make_unique<DecoderWorker>(_stream,
                                                this,
                                                endOfFileCallback,
                                                tag->num_channel == 1 ? DecoderWorker::ChannelMode::ForceStereo
                                                                      : DecoderWorker::ChannelMode::NoConversion);
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
        auto tags     = fetchTags();
        auto bitWidth = getBitWidth();
        // this is a decoder mono to stereo hack, will be removed when proper
        // transcoding implementation is added
        auto channels = tags->num_channel == 1 ? 2U : tags->num_channel;

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
