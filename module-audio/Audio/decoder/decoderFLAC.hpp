/*
 *  @file decoderFLAC.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 06.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef UNTITLED1_DECODERFLAC_HPP
#define UNTITLED1_DECODERFLAC_HPP

#include "decoder.hpp"

#include "dr_flac.h"

class decoderFLAC : public decoder {

public:
    decoderFLAC(const char *fileName);

    ~decoderFLAC();

    std::unique_ptr<Tags> fetchTags() override;

    uint32_t decode(uint32_t samplesToRead, int16_t *pcmData) override;

    void setPosition(float pos) override;


private:
    std::unique_ptr<Tags> tags;

    drflac *flac = nullptr;

    uint32_t totalSamplesCount = 0;


    /* Data encoded in UTF-8 */
    void flac_parse_text(uint8_t *in, uint32_t taglen, uint32_t datalen, uint8_t *out, uint32_t outlen);

    // Callback for when data needs to be read from the client.
    //
    // pUserData   [in]  The user data that was passed to drflac_open() and family.
    // pBufferOut  [out] The output buffer.
    // bytesToRead [in]  The number of bytes to read.
    //
    // Returns the number of bytes actually read.
    //
    // A return value of less than bytesToRead indicates the end of the stream. Do _not_ return from this callback until
    // either the entire bytesToRead is filled or you have reached the end of the stream.
    static size_t drflac_read(void *pUserData, void *pBufferOut, size_t bytesToRead);

    // Callback for when data needs to be seeked.
    //
    // pUserData [in] The user data that was passed to drflac_open() and family.
    // offset    [in] The number of bytes to move, relative to the origin. Will never be negative.
    // origin    [in] The origin of the seek - the current position or the start of the stream.
    //
    // Returns whether or not the seek was successful.
    //
    // The offset will never be negative. Whether or not it is relative to the beginning or current position is determined
    // by the "origin" parameter which will be either drflac_seek_origin_start or drflac_seek_origin_current.
    static drflac_bool32 drflac_seek(void *pUserData, int offset, drflac_seek_origin origin);

    // Use pMetadata->type to determine which metadata block is being handled and how to read the data.
    static void drflac_meta(void *pUserData, drflac_metadata *pMetadata);

};


#endif //UNTITLED1_DECODERFLAC_HPP
