// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Endpoint.hpp"

#include <cassert> // assert

using namespace audio;

void Endpoint::setStream(Stream &stream)
{
    assert(_stream == nullptr);
    _stream = &stream;
}

Stream *Endpoint::getStream() const noexcept
{
    return _stream;
}

void Endpoint::unsetStream()
{
    assert(_stream != nullptr);
    _stream = nullptr;
}

bool Endpoint::isConnected() const noexcept
{
    return _stream != nullptr;
}

void Source::connect(Sink &sink, Stream &stream)
{
    connectedSink = &sink;
    connectedSink->setStream(stream);
    setStream(stream);
}

void Source::disconnectStream()
{
    unsetStream();
    connectedSink->unsetStream();
    connectedSink = nullptr;
}
