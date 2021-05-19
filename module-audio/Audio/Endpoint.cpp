// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Endpoint.hpp"

#include <algorithm>
#include <vector>

#include <cassert> // assert

using audio::AbstractStream;
using audio::Endpoint;
using audio::Sink;
using audio::Source;
using audio::StreamConnection;

void Endpoint::connectStream(AbstractStream &stream)
{
    assert(_stream == nullptr);
    _stream = &stream;
}

void Endpoint::disconnectStream()
{
    assert(_stream != nullptr);
    _stream = nullptr;
}

bool Endpoint::isConnected() const noexcept
{
    return _stream != nullptr;
}

auto Endpoint::isFormatSupported(const AudioFormat &format) -> bool
{
    const auto &formats = getSupportedFormats();
    return std::find(std::begin(formats), std::end(formats), format) != std::end(formats);
}

StreamConnection::StreamConnection(Source *source, Sink *sink, AbstractStream *stream)
    : _sink(sink), _source(source), _stream(stream)
{
    assert(_sink != nullptr);
    assert(_source != nullptr);
    assert(_stream != nullptr);

    _sink->connectStream(*_stream);
    _source->connectStream(*_stream);
}

StreamConnection::~StreamConnection()
{
    destroy();
}

void StreamConnection::destroy()
{
    disable();
    _sink->disconnectStream();
    _source->disconnectStream();
}

void StreamConnection::enable()
{
    if (enabled) {
        return;
    }

    _stream->reset();
    _sink->enableOutput();
    _source->enableInput();

    enabled = true;
}

void StreamConnection::disable()
{
    if (!enabled) {
        return;
    }

    _source->disableInput();
    _sink->disableOutput();
    _stream->reset();

    enabled = false;
}

bool StreamConnection::isEnabled() const noexcept
{
    return enabled;
}

Source *StreamConnection::getSource() const noexcept
{
    return _source;
}

Sink *StreamConnection::getSink() const noexcept
{
    return _sink;
}

AbstractStream *StreamConnection::getStream() const noexcept
{
    return _stream;
}
