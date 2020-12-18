// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Stream.hpp"

namespace audio
{
    class Endpoint
    {
      public:
        void setStream(Stream &stream);
        Stream *getStream() const noexcept;
        void unsetStream();
        bool isConnected() const noexcept;

      private:
        Stream *_stream = nullptr;
    };

    class Sink : public Endpoint
    {};

    class Source : public Endpoint
    {
      public:
        void connect(Sink &sink, Stream &stream);
        void disconnectStream();

      private:
        Sink *connectedSink = nullptr;
    };

}; // namespace audio
