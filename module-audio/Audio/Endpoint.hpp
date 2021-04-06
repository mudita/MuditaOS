// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Stream.hpp"
#include "AudioFormat.hpp"

#include <vector>

#include <cstdint>

namespace audio
{
    class Endpoint
    {
      public:
        struct Capabilities
        {
            bool usesDMA             = false;
            std::size_t minBlockSize = 1;
            std::size_t maxBlockSize = SIZE_MAX;
        };

        Endpoint() = default;
        Endpoint(const Capabilities &caps);

        void connectStream(Stream &stream);
        void disconnectStream();
        bool isConnected() const noexcept;

        [[nodiscard]] const Capabilities &getCapabilities() const noexcept;

        auto isFormatSupported(const AudioFormat &format) -> bool;
        virtual auto getSupportedFormats() -> const std::vector<AudioFormat> & = 0;

      protected:
        Capabilities _caps;
        Stream *_stream = nullptr;
    };

    class Sink : public Endpoint
    {
      public:
        Sink() = default;
        explicit Sink(const Capabilities &caps);

        virtual void onDataSend()    = 0;
        virtual void enableOutput()  = 0;
        virtual void disableOutput() = 0;
    };

    class Source : public Endpoint
    {
      public:
        Source() = default;
        explicit Source(const Capabilities &caps);

        virtual auto getSourceFormat() -> AudioFormat;
        virtual void onDataReceive() = 0;
        virtual void enableInput()   = 0;
        virtual void disableInput()  = 0;
    };

    class IOProxy : public Source, public Sink
    {
      public:
        IOProxy() = default;
        IOProxy(const Capabilities &sourceCaps, const Capabilities &sinkCaps);

        inline bool isSinkConnected() const noexcept
        {
            return Sink::isConnected();
        }

        inline bool isSourceConnected() const noexcept
        {
            return Source::isConnected();
        }

        inline void connectOutputStream(Stream &stream)
        {
            Sink::connectStream(stream);
        }

        inline void connectInputStream(Stream &stream)
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
        StreamConnection(Source *source, Sink *sink, Stream *stream);
        ~StreamConnection();

        void enable();
        void disable();
        void destroy();

        [[nodiscard]] Source *getSource() const noexcept;
        [[nodiscard]] Sink *getSink() const noexcept;
        [[nodiscard]] Stream *getStream() const noexcept;

        [[nodiscard]] bool isEnabled() const noexcept;

      private:
        bool enabled    = false;
        Sink *_sink     = nullptr;
        Source *_source = nullptr;
        Stream *_stream = nullptr;
    };
}; // namespace audio
