// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <pulse/pulseaudio.h>
#include <Audio/AudioFormat.hpp>
#include <Audio/AbstractStream.hpp>

#include <cstdint>
#include <functional>
#include <deque>
#include <vector>

namespace audio
{
    class PulseAudioWrapper
    {
      public:
        using WriteCallback = std::function<void(const std::size_t size)>;
        PulseAudioWrapper(WriteCallback write_cb, AudioFormat audio_format);
        ~PulseAudioWrapper();

        void insert(audio::AbstractStream::Span span);
        void consume();
        std::size_t bytes() const;

      private:
        void context_state_callback();
        void *worker();
        void stream_write_cb(size_t length);
        void quit(int ret = 0);

        pthread_t tid{};
        pa_stream *stream{nullptr};
        pa_mainloop *mainloop{nullptr};
        pa_mainloop_api *mainloop_api{nullptr};
        pa_context *context{nullptr};

        WriteCallback write_cb;
        AudioFormat audio_format;

        std::vector<std::uint8_t> cache{};
        std::uint32_t cache_pos{};
    };

} // namespace audio
