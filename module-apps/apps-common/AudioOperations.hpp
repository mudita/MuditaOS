// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/decoder/Decoder.hpp>

#include <AsyncTask.hpp>

namespace app
{
    class ApplicationCommon;

    class AbstractAudioOperations
    {
      public:
        using OnPlayCallback   = std::function<void(audio::RetCode retCode, audio::Token token)>;
        using OnStopCallback   = OnPlayCallback;
        using OnPauseCallback  = OnPlayCallback;
        using OnResumeCallback = OnPlayCallback;

        virtual ~AbstractAudioOperations() noexcept = default;

        virtual bool play(const std::string &filePath, const OnPlayCallback &callback)   = 0;
        virtual bool pause(const audio::Token &token, const OnPauseCallback &callback)   = 0;
        virtual bool resume(const audio::Token &token, const OnResumeCallback &callback) = 0;
        virtual bool stop(const audio::Token &token, const OnStopCallback &callback)     = 0;
    };

    class AsyncAudioOperations : public AbstractAudioOperations, public app::AsyncCallbackReceiver
    {
      public:
        explicit AsyncAudioOperations(ApplicationCommon *application);

        bool play(const std::string &filePath, const OnPlayCallback &callback) override;
        bool pause(const audio::Token &token, const OnPauseCallback &callback) override;
        bool resume(const audio::Token &token, const OnResumeCallback &callback) override;
        bool stop(const audio::Token &token, const OnStopCallback &callback) override;

      private:
        ApplicationCommon *application = nullptr;
    };
} // namespace app
