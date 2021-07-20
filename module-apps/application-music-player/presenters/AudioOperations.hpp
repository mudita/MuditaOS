// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <Audio/decoder/Decoder.hpp>

#include <AsyncTask.hpp>

namespace app::music_player
{
    class AbstractAudioOperations
    {
      public:
        using OnPlayCallback = std::function<void(audio::Token token)>;
        using OnStopCallback = OnPlayCallback;

        virtual ~AbstractAudioOperations() noexcept = default;

        virtual bool play(const std::string &filePath, const OnPlayCallback &callback) = 0;
        virtual bool pause(const audio::Token &token)                                  = 0;
        virtual bool resume(const audio::Token &token)                                 = 0;
        virtual bool stop(const audio::Token &token, const OnStopCallback &callback)   = 0;
    };

    class AudioOperations : public AbstractAudioOperations, public app::AsyncCallbackReceiver
    {
      public:
        explicit AudioOperations(Application *application);

        bool play(const std::string &filePath, const OnPlayCallback &callback) override;
        bool pause(const audio::Token &token) override;
        bool resume(const audio::Token &token) override;
        bool stop(const audio::Token &token, const OnStopCallback &callback) override;

      private:
        Application *application = nullptr;
    };
} // namespace app::music_player
