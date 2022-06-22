// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <pulse/pulseaudio.h>
#include <Audio/AudioFormat.hpp>
#include <Audio/AbstractStream.hpp>

#include <cstdint>
#include <functional>
#include <deque>
#include <vector>
#include <memory>

namespace audio::pulse_audio
{
    class Stream
    {
      public:
        Stream(AudioFormat audio_format_, pa_context *ctx);
        ~Stream();

        void insert(AbstractStream::Span span);
        void consume();
        std::size_t bytes() const;

        pa_stream *raw();

      private:
        std::vector<std::uint8_t> cache{};
        std::uint32_t cache_pos{};
        AudioFormat audio_format;
        pa_stream *stream{nullptr};
    };

    class Context
    {
      public:
        using WriteCallback = std::function<void(const std::size_t size)>;
        Context();
        ~Context();

        Stream *open_stream(AudioFormat audio_format_, WriteCallback write_cb);
        void close_stream();

      private:
        void context_state_callback();
        void *worker();
        void stream_write_cb(size_t length);
        void quit(int ret = 0);

        pthread_t tid{};
        pa_mainloop *mainloop{nullptr};
        pa_mainloop_api *mainloop_api{nullptr};
        pa_context *context{nullptr};
        std::unique_ptr<Stream> stream;

        WriteCallback write_cb;
    };

} // namespace audio::pulse_audio
