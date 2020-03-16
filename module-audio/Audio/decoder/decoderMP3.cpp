/*
 *  @file decoderMP3.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 08.04.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_SIMD
#define MINIMP3_IMPLEMENTATION

#include "decoderMP3.hpp"

#include "atag.hpp"

#include <array>

namespace audio
{

    decoderMP3::decoderMP3(const char *fileName) : decoder(fileName), tag(std::make_unique<Tags>())
    {

        if (fileSize == 0) {
            return;
        }

        mp3d = static_cast<mp3dec_t *>(malloc(sizeof(mp3dec_t)));

        mp3dec_init(mp3d);

        if (!find_first_valid_frame()) {
            return;
        }

        isInitialized = true;
    }

    decoderMP3::~decoderMP3()
    {
        mp3dec_deinit(mp3d);
        free(mp3d);
    }

    void decoderMP3::setPosition(float pos)
    {
        decoderNotFirstRun = false;

        vfs.fseek(fd, (pos * fileSize) + firstValidFrameFileOffset, SEEK_SET);

        // TODO: M.P Currently calculating MP3 position is unsupported, in general seeking is supported though.
        // position += (float) ((float) (samplesToReadChann / chanNumber) / (float) sampleRate);
    }

    std::unique_ptr<Tags> decoderMP3::fetchTags()
    {

        vfs.fseek(fd, firstValidFrameFileOffset + 4, SEEK_SET);
        auto buff = std::make_unique<uint8_t[]>(firstValidFrameByteSize);

        vfs.fread(buff.get(), 1, firstValidFrameByteSize, fd);

        xing_info_t xinfo = {};
        if (parseXingHeader(buff.get(), firstValidFrameByteSize, &xinfo)) {
            // valid xing header found
            tag->total_duration_s = xinfo.TotalFrames * (samplesPerFrame) / sampleRate;
        }
        else {
            // Scan through whole file and count frames
            uint32_t frames_count = get_frames_count();
            tag->total_duration_s = frames_count * (samplesPerFrame) / sampleRate;
        }

        tag->num_channel = chanNumber;
        tag->sample_rate = sampleRate;
        tag->filePath.append(filePath);

        tag->duration_min  = tag->total_duration_s / 60;
        tag->duration_hour = tag->duration_min / 60;
        tag->duration_sec  = tag->total_duration_s % 60;

        vfs.rewind(fd);

        // Parse ID3 tags
        // Allocate buffer for fetching two parts of MP3: init and last, 512Kbytes should be sufficient to fetch all
        // necessary tags
        auto source = std::make_unique<std::array<char, 512 * 1024>>();

        // Read beginning of MP3(256kbytes)
        vfs.fread(&(*source)[0], 1, source->size() / 2, fd);

        // Read last section of MP3(256kbytes)
        vfs.fseek(fd, -(source->size() / 2), SEEK_END);
        vfs.fread(&(*source)[source->size() / 2], 1, source->size() / 2, fd);

        if (atag::id3v2::is_tagged(*source)) {

            atag::simple_tag idtag = atag::id3v2::simple_parse(*source);

            tag->title  = idtag.title;
            tag->artist = idtag.artist;
            tag->album  = idtag.album;
            tag->year   = std::to_string(idtag.year);
        }

        // If title tag empty fill it with raw file name
        if (tag->title.size() == 0) {
            auto pos = filePath.rfind("/");
            if (pos == std::string::npos) {
                tag->title.append(filePath);
            }
            else {
                tag->title.append(&filePath[pos + 1]);
            }
        }

        vfs.rewind(fd);

        return std::make_unique<Tags>(*tag);
    }

    bool decoderMP3::find_first_valid_frame()
    {

        mp3dec_frame_info_t info = {};

        int32_t bytesAvailable = DECODER_BUFFER_SIZE;
        uint32_t bufferIndex   = 0;

        auto decBuffer = std::make_unique<uint8_t[]>(DECODER_BUFFER_SIZE);

        vfs.rewind(fd);

        if (vfs.fread(decBuffer.get(), 1, DECODER_BUFFER_SIZE, fd) == 0) {
            return false;
        }

    refill:

        // refill buffer if necessary(only if over 87,5% of bytes are consumed)
        if (bufferIndex > (DECODER_BUFFER_SIZE - (DECODER_BUFFER_SIZE / 8))) {
            memcpy(&decBuffer[0], &decBuffer[bufferIndex], bytesAvailable);
            uint32_t bytesRead = vfs.fread(&decBuffer[bytesAvailable], 1, DECODER_BUFFER_SIZE - bytesAvailable, fd);

            if (bytesRead == 0) {
                return false;
            }

            bytesAvailable += bytesRead;
            bufferIndex = 0;
        }

        while (1) {

            uint32_t smpl = mp3dec_find_frame(mp3d, &decBuffer[bufferIndex], bytesAvailable, &info);
            bufferIndex += info.frame_bytes;
            bytesAvailable -= info.frame_bytes;

            // Valid frame
            if (smpl && info.frame_bytes) {

                // Fill necessary parameters
                samplesPerFrame           = smpl;
                sampleRate                = info.hz;
                chanNumber                = info.channels;
                firstValidFrameByteSize   = (144 * info.bitrate_kbps * 1000 / info.hz);
                firstValidFrameFileOffset = vfs.ftell(fd) - bytesAvailable - firstValidFrameByteSize;

                vfs.rewind(fd);

                return true;
            }
            // Decoder skipped ID3 or invalid data
            else if ((smpl == 0) && info.frame_bytes) {
            }
            // insufficient data
            else if ((info.frame_bytes == 0) && (smpl == 0)) {
                goto refill;
            }
        }
    }

    uint32_t decoderMP3::get_frames_count()
    {

        int32_t bytesAvailable   = DECODER_BUFFER_SIZE;
        uint32_t frames_count    = 0;
        mp3dec_frame_info_t info = {};
        bool last_refill         = false;

        uint32_t bufferIndex = 0;
        auto decBuffer       = std::make_unique<uint8_t[]>(DECODER_BUFFER_SIZE);

        // Jump to the file beginning
        vfs.rewind(fd);

        /* Fill decBuffer */
        if (vfs.fread(decBuffer.get(), 1, DECODER_BUFFER_SIZE, fd) == 0) {
            return 0;
        }

    refill:

        // refill buffer if necessary(only if over 87,5% of bytes are consumed)
        if (bufferIndex > (DECODER_BUFFER_SIZE - (DECODER_BUFFER_SIZE / 8))) {
            memcpy(&decBuffer[0], &decBuffer[bufferIndex], bytesAvailable);
            uint32_t bytesRead =
                vfs.fread(decBuffer.get() + bytesAvailable, 1, DECODER_BUFFER_SIZE - bytesAvailable, fd);

            if (bytesRead != (DECODER_BUFFER_SIZE - bytesAvailable)) {
                last_refill = true;
            }

            bytesAvailable += bytesRead;
            bufferIndex = 0;
        }

        while (1) {

            uint32_t smpl = mp3dec_find_frame(mp3d, &decBuffer[bufferIndex], bytesAvailable, &info);
            bufferIndex += info.frame_bytes;
            bytesAvailable -= info.frame_bytes;

            // Valid frame
            if (smpl && info.frame_bytes) {
                frames_count++;
            }

            // Decoder skipped ID3 or invalid data
            if ((smpl == 0) && info.frame_bytes) {
                frames_count++;
            }

            // Last frame
            if (last_refill && (info.frame_bytes == 0) && (smpl == 0)) {
                return frames_count;
            }

            // insufficient data
            if ((info.frame_bytes == 0) && (smpl == 0)) {
                goto refill;
            }
        }
    }

    uint32_t decoderMP3::decode(uint32_t samplesToRead, int16_t *pcmData)
    {
        mp3dec_frame_info_t info = {0};

        if (!decoderNotFirstRun) {
            decoderBuffer    = std::make_unique<uint8_t[]>(DECODER_BUFFER_SIZE);
            pcmsamplesbuffer = std::make_unique<uint16_t[]>(pcmsamplesbuffer_size);

            // Fill decoderBuffer
            uint32_t bytesRead = vfs.fread(decoderBuffer.get(), 1, DECODER_BUFFER_SIZE, fd);

            if (bytesRead == 0) {
                return 0;
            }

            bytesAvailable   = DECODER_BUFFER_SIZE;
            decoderBufferIdx = 0;

            decoderNotFirstRun = true;
        }
        else if (!lastRefill) {
            bytesAvailable = DECODER_BUFFER_SIZE - decoderBufferIdx;
        }

        uint32_t samplesFetched = pcmsamplesbuffer_idx;

    refill:

        // refill buffer if necessary(only if over 87,5% of bytes consumed)
        if (!lastRefill && (decoderBufferIdx > (DECODER_BUFFER_SIZE - (DECODER_BUFFER_SIZE / 8)))) {
            memcpy(&decoderBuffer[0], &decoderBuffer[decoderBufferIdx], bytesAvailable);
            uint32_t bytesRead = vfs.fread(&decoderBuffer[bytesAvailable], 1, DECODER_BUFFER_SIZE - bytesAvailable, fd);

            if (bytesRead != (DECODER_BUFFER_SIZE - bytesAvailable)) {
                lastRefill = true;
            }

            bytesAvailable += bytesRead;
            decoderBufferIdx = 0;
        }

        uint32_t samplesToReadChann = samplesToRead;

        while (1) {
            uint32_t smpl = 0;
            if (samplesFetched < samplesToReadChann) {
                smpl = mp3dec_decode_frame(mp3d,
                                           &decoderBuffer[decoderBufferIdx],
                                           bytesAvailable,
                                           (short *)&pcmsamplesbuffer[samplesFetched],
                                           &info);
            }

            bytesAvailable -= info.frame_bytes;
            decoderBufferIdx += info.frame_bytes;

            // Valid frame
            if (smpl && info.frame_bytes) {
                samplesFetched += smpl * info.channels;
            }

            if (samplesFetched >= samplesToReadChann) {
                break;
            }

            // Insufficient data
            if (!lastRefill && (info.frame_bytes == 0) && (smpl == 0)) {
                goto refill;
            }

            if (lastRefill && (info.frame_bytes == 0) && (smpl == 0)) {
                return 0;
            }
        }

        // Copy decoded samples to dest buffer
        memcpy(pcmData, &pcmsamplesbuffer[0], samplesToReadChann * sizeof(int16_t));

        // Manage samples in internal buffer
        pcmsamplesbuffer_idx = samplesFetched - samplesToReadChann;
        memmove(&pcmsamplesbuffer[0], &pcmsamplesbuffer[samplesToReadChann], pcmsamplesbuffer_idx * sizeof(int16_t));

        /* Calculate frame duration in seconds */
        position += (float)((float)(chanNumber == 2 ? samplesToRead / chanNumber : samplesToRead) / (float)sampleRate);

        return samplesToRead;
    }

} // namespace audio