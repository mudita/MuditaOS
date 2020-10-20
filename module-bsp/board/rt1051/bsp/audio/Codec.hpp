// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_CODEC_HPP
#define PUREPHONE_CODEC_HPP

#include <memory>
#include <optional>

class CodecParams
{
  public:
};

enum class CodecRetCode
{
    Success,
    InvalidSampleRate,
    InvalidInputPath,
    InvalidOutputPath,
    InvalidArgument
};

class Codec
{
  public:
    virtual ~Codec()
    {}

    virtual std::optional<uint32_t> Probe() = 0;

    virtual CodecRetCode Start(const CodecParams &param) = 0;

    virtual CodecRetCode Pause() = 0;

    virtual CodecRetCode Resume() = 0;

    virtual CodecRetCode Stop() = 0;

    virtual CodecRetCode Ioctrl(const CodecParams &param) = 0;
};

#endif // PUREPHONE_CODEC_HPP
