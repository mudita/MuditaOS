// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PulseAudioWrapper.hpp"
#include <pthread.h>
#include <signal.h>
#include <iostream>
#include <utility>
#include <stdexcept>

namespace audio::pulse_audio
{
    Stream::Stream(AudioFormat audio_format_, pa_context *ctx) : audio_format{audio_format_}
    {
        /// PulseAudio requests at least SampleRate bytes of data to start playback
        /// Due to how module-audio's streams operate we can load a bit more data to the cache than needed.
        /// Doubling the buffer size is cheap and simple solution.
        cache.reserve(audio_format.getSampleRate() * 2);

        pa_sample_spec sample_spec;
        sample_spec.channels = audio_format.getChannels();
        sample_spec.rate     = audio_format.getSampleRate();
        sample_spec.format   = PA_SAMPLE_S16LE;

        if ((stream = pa_stream_new(ctx, "PureOSStream", &sample_spec, nullptr)) == nullptr) {
            throw std::runtime_error(pa_strerror(pa_context_errno(ctx)));
        }
    }

    Stream::~Stream()
    {
        pa_stream_disconnect(stream);
        pa_stream_unref(stream);
    }

    void Stream::insert(AbstractStream::Span span)
    {
        std::copy(span.data, span.dataEnd(), &cache[cache_pos]);
        cache_pos += span.dataSize;
    }

    void Stream::consume()
    {
        if (pa_stream_write(stream, cache.data(), cache_pos, NULL, 0, PA_SEEK_RELATIVE) < 0) {
            fprintf(stderr, "pa_stream_write() failed\n");
            return;
        }
        cache_pos = 0;
    }

    std::size_t Stream::bytes() const
    {
        return cache_pos;
    }

    pa_stream *Stream::raw()
    {
        return stream;
    }

    Context::Context()
    {
        pthread_create(
            &tid,
            nullptr,
            [](void *arg) -> void * {
                /// Any 'external' threads that are not part of the FreeRTOS simulator port must be treated as a
                /// interrupt source and have to have all the signals masked in order to not 'steal' work from the
                /// threads managed by the port.
                sigset_t set;
                sigfillset(&set);
                pthread_sigmask(SIG_SETMASK, &set, NULL);

                auto *inst = static_cast<Context *>(arg);
                return inst->worker();
            },
            this);
    }

    void *Context::worker()
    {
        mainloop     = pa_mainloop_new();
        mainloop_api = pa_mainloop_get_api(mainloop);
        context      = pa_context_new(mainloop_api, "PureOSContext");
        pa_context_set_state_callback(
            context,
            [](pa_context *, void *arg) {
                auto *inst = static_cast<Context *>(arg);
                inst->context_state_callback();
            },
            this);
        pa_context_connect(context, nullptr, {}, nullptr);

        int ret = 1;
        if (pa_mainloop_run(mainloop, &ret) < 0) {
            fprintf(stderr, "pa_mainloop_run() failed.\n");
            std::abort();
        }

        return nullptr;
    }

    void Context::context_state_callback()
    {
        assert(context);

        switch (pa_context_get_state(context)) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_READY:
            break;

        case PA_CONTEXT_TERMINATED:
            quit(0);
            fprintf(stderr, "PulseAudio connection terminated.\n");
            break;

        case PA_CONTEXT_FAILED:
        default:
            fprintf(stderr, "Connection failure: %s\n", pa_strerror(pa_context_errno(context)));
            quit(1);
        }
    }

    void Context::stream_write_cb(size_t length)
    {
        write_cb(length);
    }

    Context::~Context()
    {
        quit();
        pthread_join(tid, nullptr);

        if (context != nullptr) {
            pa_context_unref(context);
            context = nullptr;
        }

        if (mainloop != nullptr) {
            pa_mainloop_free(mainloop);
            mainloop     = nullptr;
            mainloop_api = nullptr;
        }
    }

    void Context::quit(int ret)
    {
        if (mainloop_api != nullptr) {
            mainloop_api->quit(mainloop_api, ret);
        }
    }

    Stream *Context::open_stream(AudioFormat audio_format_, WriteCallback write_cb_)
    {
        try {
            stream   = std::make_unique<Stream>(audio_format_, context);
            write_cb = write_cb_;

            pa_stream_set_write_callback(
                stream->raw(),
                [](pa_stream *, size_t length, void *arg) {
                    auto *inst = static_cast<Context *>(arg);
                    inst->stream_write_cb(length);
                },
                this);

            if (pa_stream_connect_playback(stream->raw(), nullptr, nullptr, {}, nullptr, nullptr) < 0) {
                throw std::runtime_error(pa_strerror(pa_context_errno(context)));
            }
        }
        catch (const std::runtime_error &e) {
            fprintf(stderr, e.what());
            return nullptr;
        }

        return stream.get();
    }
    void Context::close_stream()
    {
        stream = {};
    }
} // namespace audio::pulse_audio
