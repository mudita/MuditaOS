// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/Endpoint.hpp>
#include <Audio/AudioFormat.hpp>

#include <vector>

namespace audio::test
{
    constexpr inline auto testTraits = ::audio::Endpoint::Traits{};

    class TestSink : public audio::Sink
    {
      public:
        explicit TestSink(std::vector<AudioFormat> supportedFormats);

        void onDataSend() override;
        void enableOutput() override;
        void disableOutput() override;
        auto getSupportedFormats() -> std::vector<AudioFormat> override;
        auto getTraits() const -> ::audio::Endpoint::Traits override;

      private:
        std::vector<AudioFormat> formats;
    };

    class TestSource : public audio::Source
    {
      public:
        explicit TestSource(AudioFormat audioFormat);
        explicit TestSource(std::vector<AudioFormat> supportedFormats, AudioFormat sourceFormat);

        auto getSourceFormat() -> AudioFormat override;
        void onDataReceive() override;
        void enableInput() override;
        void disableInput() override;
        auto getSupportedFormats() -> std::vector<AudioFormat> override;
        auto getTraits() const -> ::audio::Endpoint::Traits override;

      private:
        std::vector<AudioFormat> formats;
        AudioFormat sourceFormat;
    };

} // namespace audio::test
