// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EncoderWAV.hpp"
#include <log/log.hpp>

namespace audio
{

    EncoderWAV::EncoderWAV(const char *fileName, const Encoder::Format &frmt) : Encoder(fileName, frmt)
    {

        WAVE_FormatTypeDef WaveFormat = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        /* Initialize the encoder structure */
        WaveFormat.SampleRate    = format.sampleRate; /* Audio sampling frequency */
        WaveFormat.NbrChannels   = format.chanNr;     /* Number of channels: 1:Mono or 2:Stereo */
        WaveFormat.BitPerSample  = 16;                /* Number of bits per sample (16, 24 or 32) */
        WaveFormat.FileSize      = 0x001D4C00;        /* Total length of useful audio data (payload) */
        WaveFormat.SubChunk1Size = 44;                /* The file header chunk size */
        WaveFormat.ByteRate      = (WaveFormat.SampleRate * (WaveFormat.BitPerSample / 8) *
                               WaveFormat.NbrChannels); /* Number of bytes per second  (sample rate * block align)  */
        WaveFormat.BlockAlign = WaveFormat.NbrChannels * (WaveFormat.BitPerSample / 8); /* channels * bits/sample / 8 */

        HeaderInit(WaveFormat);
    }

    EncoderWAV::~EncoderWAV()
    {
        std::rewind(fd);
        /* Update the wav file header save it into wav file */
        HeaderUpdate();
        if (std::fwrite(pHeaderBuff, 1, sizeof(WAVE_FormatTypeDef), fd) != sizeof(WAVE_FormatTypeDef)) {
            LOG_ERROR("Updating WAV header failed");
        }
    }

    uint32_t EncoderWAV::Encode(uint32_t samplesToWrite, int16_t *pcmData)
    {
        /*
         * Write int16_t PCM samples to file.
         */
        auto byteswritten = std::fwrite(pcmData, sizeof(int16_t), samplesToWrite, fd);
        if (byteswritten != samplesToWrite) {
            return 0;
        }

        /* Calculate frame duration in seconds */
        position += (float)((float)(samplesToWrite / format.chanNr) / (float)(format.sampleRate));
        return byteswritten;
    }

    void EncoderWAV::HeaderInit(const EncoderWAV::WAVE_FormatTypeDef &pWaveFormatStruct)
    {
        /* Write chunkID, must be 'RIFF'  ------------------------------------------*/
        pHeaderBuff[0] = 'R';
        pHeaderBuff[1] = 'I';
        pHeaderBuff[2] = 'F';
        pHeaderBuff[3] = 'F';

        /* Write the file length ---------------------------------------------------*/
        /* The sampling time: this value will be written back at the end of the
           recording operation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
        pHeaderBuff[4] = 0x00;
        pHeaderBuff[5] = 0x4C;
        pHeaderBuff[6] = 0x1D;
        pHeaderBuff[7] = 0x00;
        /* Write the file format, must be 'WAVE' -----------------------------------*/
        pHeaderBuff[8]  = 'W';
        pHeaderBuff[9]  = 'A';
        pHeaderBuff[10] = 'V';
        pHeaderBuff[11] = 'E';

        /* Write the format chunk, must be'fmt ' -----------------------------------*/
        pHeaderBuff[12] = 'f';
        pHeaderBuff[13] = 'm';
        pHeaderBuff[14] = 't';
        pHeaderBuff[15] = ' ';

        /* Write the length of the 'fmt' data, must be 0x10 ------------------------*/
        pHeaderBuff[16] = 0x10;
        pHeaderBuff[17] = 0x00;
        pHeaderBuff[18] = 0x00;
        pHeaderBuff[19] = 0x00;

        /* Write the audio format, must be 0x01 (PCM) ------------------------------*/
        pHeaderBuff[20] = 0x01;
        pHeaderBuff[21] = 0x00;

        /* Write the number of channels, ie. 0x01 (Mono) ---------------------------*/
        pHeaderBuff[22] = pWaveFormatStruct.NbrChannels;
        pHeaderBuff[23] = 0x00;

        /* Write the Sample Rate in Hz ---------------------------------------------*/
        /* Write Little Endian ie. 8000 = 0x00001F40 => byte[24]=0x40, byte[27]=0x00*/
        pHeaderBuff[24] = (uint8_t)((pWaveFormatStruct.SampleRate & 0xFF));
        pHeaderBuff[25] = (uint8_t)((pWaveFormatStruct.SampleRate >> 8) & 0xFF);
        pHeaderBuff[26] = (uint8_t)((pWaveFormatStruct.SampleRate >> 16) & 0xFF);
        pHeaderBuff[27] = (uint8_t)((pWaveFormatStruct.SampleRate >> 24) & 0xFF);

        /* Write the Byte Rate -----------------------------------------------------*/
        pHeaderBuff[28] = (uint8_t)((pWaveFormatStruct.ByteRate & 0xFF));
        pHeaderBuff[29] = (uint8_t)((pWaveFormatStruct.ByteRate >> 8) & 0xFF);
        pHeaderBuff[30] = (uint8_t)((pWaveFormatStruct.ByteRate >> 16) & 0xFF);
        pHeaderBuff[31] = (uint8_t)((pWaveFormatStruct.ByteRate >> 24) & 0xFF);

        /* Write the block alignment -----------------------------------------------*/
        pHeaderBuff[32] = pWaveFormatStruct.BlockAlign;
        pHeaderBuff[33] = 0x00;

        /* Write the number of bits per sample -------------------------------------*/
        pHeaderBuff[34] = pWaveFormatStruct.BitPerSample;
        pHeaderBuff[35] = 0x00;

        /* Write the Data chunk, must be 'data' ------------------------------------*/
        pHeaderBuff[36] = 'd';
        pHeaderBuff[37] = 'a';
        pHeaderBuff[38] = 't';
        pHeaderBuff[39] = 'a';

        /* Write the number of sample data -----------------------------------------*/
        /* This variable will be written back at the end of the recording operation */
        pHeaderBuff[40] = 0x00;
        pHeaderBuff[41] = 0x4C;
        pHeaderBuff[42] = 0x1D;
        pHeaderBuff[43] = 0x00;
    }

    void EncoderWAV::HeaderUpdate()
    {
        /* Write the file length ---------------------------------------------------*/
        /* The sampling time: this value will be written back at the end of the
           recording operation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
        pHeaderBuff[4] = (uint8_t)(fileSize);
        pHeaderBuff[5] = (uint8_t)(fileSize >> 8);
        pHeaderBuff[6] = (uint8_t)(fileSize >> 16);
        pHeaderBuff[7] = (uint8_t)(fileSize >> 24);
        /* Write the number of sample data -----------------------------------------*/
        /* This variable will be written back at the end of the recording operation */
        fileSize -= 44;
        pHeaderBuff[40] = (uint8_t)(fileSize);
        pHeaderBuff[41] = (uint8_t)(fileSize >> 8);
        pHeaderBuff[42] = (uint8_t)(fileSize >> 16);
        pHeaderBuff[43] = (uint8_t)(fileSize >> 24);
    }

} // namespace audio
