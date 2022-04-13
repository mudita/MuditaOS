// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PulseAudioWrapper.hpp"
#include <pthread.h>
#include <iostream>
#include <utility>

namespace audio
{
    PulseAudioWrapper::PulseAudioWrapper(WriteCallback write_cb, AudioFormat audio_format)
        : write_cb{std::move(write_cb)}, audio_format(audio_format)
    {
        /// PulseAudio requests at least SampleRate bytes of data to start playback
        /// Due to how module-audio's streams operate we can load a bit more data to the cache than needed.
        /// Doubling the buffer size is cheap and simple solution.
        cache.reserve(audio_format.getSampleRate() * 2);

        pthread_create(
            &tid,
            nullptr,
            [](void *arg) -> void * {
                /// Mask all available signals to not interfere with the FreeRTOS simulator port
                sigset_t set;
                sigfillset(&set);
                pthread_sigmask(SIG_SETMASK, &set, NULL);

                auto *inst = static_cast<PulseAudioWrapper *>(arg);
                return inst->worker();
            },
            this);
    }

    void *PulseAudioWrapper::worker()
    {
        mainloop     = pa_mainloop_new();
        mainloop_api = pa_mainloop_get_api(mainloop);
        context      = pa_context_new(mainloop_api, "PureOSContext");
        pa_context_set_state_callback(
            context,
            [](pa_context *, void *arg) {
                auto *inst = static_cast<PulseAudioWrapper *>(arg);
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

    void PulseAudioWrapper::context_state_callback()
    {
        assert(context);

        switch (pa_context_get_state(context)) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY: {
            int r;
            pa_sample_spec sample_spec;
            sample_spec.channels = audio_format.getChannels();
            sample_spec.rate     = audio_format.getSampleRate();
            sample_spec.format   = PA_SAMPLE_S16LE;

            if ((stream = pa_stream_new(context, "PureOSStream", &sample_spec, nullptr)) == nullptr) {
                fprintf(stderr, "pa_stream_new() failed: %s\n", pa_strerror(pa_context_errno(context)));
                return;
            }

            pa_stream_set_write_callback(
                stream,
                [](pa_stream *, size_t length, void *arg) {
                    auto *inst = static_cast<PulseAudioWrapper *>(arg);
                    inst->stream_write_cb(length);
                },
                this);

            if ((r = pa_stream_connect_playback(stream, nullptr, nullptr, {}, nullptr, nullptr)) < 0) {
                fprintf(stderr, "pa_stream_connect_playback() failed: %s\n", pa_strerror(pa_context_errno(context)));
                return;
            }

            break;
        }

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

    void PulseAudioWrapper::stream_write_cb(size_t length)
    {
        write_cb(length);
    }

    PulseAudioWrapper::~PulseAudioWrapper()
    {
        quit();
        pthread_join(tid, nullptr);

        if (stream != nullptr) {
            pa_stream_unref(stream);
        }

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

    void PulseAudioWrapper::quit(int ret)
    {
        if (mainloop_api != nullptr) {
            mainloop_api->quit(mainloop_api, ret);
        }
    }

    void PulseAudioWrapper::insert(audio::AbstractStream::Span span)
    {
        std::copy(span.data, span.dataEnd(), &cache[cache_pos]);
        cache_pos += span.dataSize;
    }

    void PulseAudioWrapper::consume()
    {
        if (pa_stream_write(stream, cache.data(), cache_pos, NULL, 0, PA_SEEK_RELATIVE) < 0) {
            fprintf(stderr, "pa_stream_write() failed\n");
            return;
        }
        cache_pos = 0;
    }

    std::size_t PulseAudioWrapper::bytes() const
    {
        return cache_pos;
    }
} // namespace audio
