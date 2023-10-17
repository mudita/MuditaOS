// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractStream.hpp"
#include "AudioFormat.hpp"

#include <chrono>
#include <optional>
#include <vector>

#include <cstdint>

namespace audio
{
    class Endpoint
    {
      public:
        struct Traits
        {
            bool usesDMA = false;

            // optional constraints
            std::optional<std::size_t> blockSizeConstraint          = std::nullopt;
            std::optional<std::chrono::milliseconds> timeConstraint = std::nullopt;
        };

        Endpoint() = default;

        void connectStream(AbstractStream &stream);
        void disconnectStream();
        bool isConnected() const noexcept;

        [[nodiscard]] virtual auto getTraits() const -> Traits = 0;

        auto isFormatSupported(const AudioFormat &format) -> bool;
        virtual auto getSupportedFormats() -> std::vector<AudioFormat> = 0;

      protected:
        AbstractStream *_stream = nullptr;
    };

    class Sink : public Endpoint
    {
      public:
        virtual auto getSinkFormat() -> AudioFormat = 0;
        virtual void onDataSend()                   = 0;
        virtual void enableOutput()                 = 0;
        virtual void disableOutput()                = 0;
    };

    class Source : public Endpoint
    {
      public:
        virtual auto getSourceFormat() -> AudioFormat = 0;
        virtual void onDataReceive()                  = 0;
        virtual void enableInput()                    = 0;
        virtual void disableInput()                   = 0;
    };

    class IOProxy : public Source, public Sink
    {
      public:
        inline bool isSinkConnected() const noexcept
        {
            return Sink::isConnected();
        }

        inline bool isSourceConnected() const noexcept
        {
            return Source::isConnected();
        }

        inline void connectOutputStream(AbstractStream &stream)
        {
            Sink::connectStream(stream);
        }

        inline void connectInputStream(AbstractStream &stream)
        {
            Source::connectStream(stream);
        }

        inline auto isFormatSupportedBySink(const AudioFormat &format) -> bool
        {
            return Sink::isFormatSupported(format);
        }

        inline auto isFormatSupportedBySource(const AudioFormat &format) -> bool
        {
            return Source::isFormatSupported(format);
        }
    };

    class StreamConnection
    {
      public:
        StreamConnection() = default;
        StreamConnection(Source *source, Sink *sink, AbstractStream *stream);
        ~StreamConnection();

        void enable();
        void disable();
        void destroy();

        [[nodiscard]] Source *getSource() const noexcept;
        [[nodiscard]] Sink *getSink() const noexcept;
        [[nodiscard]] AbstractStream *getStream() const noexcept;
        [[nodiscard]] bool isEnabled() const noexcept;

      private:
        bool enabled            = false;
        Sink *_sink             = nullptr;
        Source *_source         = nullptr;
        AbstractStream *_stream = nullptr;
    };
}; // namespace audio
