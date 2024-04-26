// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AudioModel.hpp>
#include <Timers/TimerHandle.hpp>

namespace app
{
    class ApplicationCommon;
    class AbstractFrontlightModel;
} // namespace app

namespace app
{
    class AbstractDemoModel
    {
      public:
        virtual ~AbstractDemoModel() noexcept = default;

        virtual void Start()             = 0;
        virtual void Stop()              = 0;
        virtual bool IsActivated() const = 0;
    };

    class DemoModel : public AbstractDemoModel
    {
        using OnEndDemoCallback = std::function<void()>;

      public:
        DemoModel(app::ApplicationCommon *app,
                  AbstractAudioModel &audioModel,
                  AbstractFrontlightModel &frontlightModel,
                  const std::vector<std::string> &filesPath,
                  OnEndDemoCallback callback);
        void Start() override;
        void Stop() override;
        bool IsActivated() const override;

      private:
        AbstractAudioModel &audioModel;
        AbstractFrontlightModel &frontlightModel;
        std::vector<std::string> filesPath;
        OnEndDemoCallback endDemoCallback{nullptr};
        bool activated{false};
        sys::TimerHandle timer;
        std::uint8_t nextTrackIndex{0};

        void PlayNextSong();
        void StopAudioPlayback();
    };
} // namespace app
