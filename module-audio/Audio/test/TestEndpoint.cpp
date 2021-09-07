// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TestEndpoint.hpp"

#include <Audio/AudioFormat.hpp>

using audio::AudioFormat;
using audio::test::TestSink;
using audio::test::TestSource;

TestSink::TestSink(std::vector<AudioFormat> supportedFormats) : formats(std::move(supportedFormats))
{}

auto TestSink::getSupportedFormats() -> std::vector<AudioFormat>
{
    return formats;
}

void TestSink::onDataSend()
{}

void TestSink::enableOutput()
{}

void TestSink::disableOutput()
{}

auto TestSink::getTraits() const -> ::audio::Endpoint::Traits
{
    return testTraits;
}

TestSource::TestSource(std::vector<AudioFormat> supportedFormats, AudioFormat sourceFormat)
    : formats(std::move(supportedFormats)), sourceFormat(std::move(sourceFormat))
{}

TestSource::TestSource(AudioFormat audioFormat)
    : formats(std::vector<AudioFormat>{audioFormat}), sourceFormat(std::move(audioFormat))
{}

auto TestSource::getSourceFormat() -> AudioFormat
{
    return sourceFormat;
}

void TestSource::onDataReceive()
{}

void TestSource::enableInput()
{}

void TestSource::disableInput()
{}

auto TestSource::getSupportedFormats() -> std::vector<AudioFormat>
{
    return formats;
}

auto TestSource::getTraits() const -> ::audio::Endpoint::Traits
{
    return testTraits;
}
