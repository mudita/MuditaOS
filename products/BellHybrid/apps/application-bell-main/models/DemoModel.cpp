// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DemoModel.hpp"
#include "models/MainFrontlightModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <Timers/TimerFactory.hpp>

namespace
{
    constexpr auto demoModeDuration{std::chrono::minutes(5)};
    constexpr auto frontlightFadeInDuration{std::chrono::seconds(45)};

    std::uint8_t getNextTrackIndex(const std::vector<std::string> &tracks, std::uint8_t currentIndex)
    {
        return (currentIndex >= tracks.size() - 1) ? 0 : ++currentIndex;
    }
} // namespace

namespace app
{
    DemoModel::DemoModel(app::ApplicationCommon *app,
                         AbstractAudioModel &audioModel,
                         AbstractFrontlightModel &frontlightModel,
                         const std::vector<std::string> &filesPath,
                         OnEndDemoCallback callback)
        : audioModel{audioModel}, frontlightModel{frontlightModel}, filesPath{filesPath}, endDemoCallback{
                                                                                              std::move(callback)}
    {
        timer = sys::TimerFactory::createSingleShotTimer(app, "demoModeTimer", demoModeDuration, [this](sys::Timer &) {
            StopAudioPlayback();
            if (endDemoCallback != nullptr) {
                endDemoCallback();
            }
        });
    }

    void DemoModel::Start()
    {
        LOG_INFO("Demo Mode start");
        activated = true;
        PlayNextSong();
        frontlightModel.LockKeypressTrigger();
        frontlightModel.StartBrightnessFadeIn(frontlightFadeInDuration);
        timer.restart(demoModeDuration);
    }

    void DemoModel::Stop()
    {
        activated = false;
        StopAudioPlayback();
        frontlightModel.UnlockKeypressTrigger();
        frontlightModel.TurnOff();
        timer.stop();
        LOG_INFO("Demo Mode stop");
    }

    void DemoModel::PlayNextSong()
    {
        if (filesPath.empty()) {
            return;
        }
        using Status         = AbstractAudioModel::PlaybackFinishStatus;
        auto onStartCallback = [this](audio::RetCode retCode) {
            if (retCode != audio::RetCode::Success) {
                LOG_ERROR("Failed to start audio in demo mode!");
            }
        };
        auto onFinishedCallback = [this](Status status) {
            if (status == Status::Error) {
                LOG_ERROR("Audio playback in demo mode ended with an error!");
            }
        };
        audioModel.setPlaybackFinishedCb(std::move(onFinishedCallback));
        audioModel.play(filesPath[nextTrackIndex],
                        AbstractAudioModel::PlaybackType::Alarm, // only for Alarm there is a loop
                        std::move(onStartCallback),
                        audio::FadeIn::Enable);
        nextTrackIndex = getNextTrackIndex(filesPath, nextTrackIndex);
    }

    void DemoModel::StopAudioPlayback()
    {
        audioModel.stopPlayedByThis({});
    }

    bool DemoModel::IsActivated() const
    {
        return activated;
    }

} // namespace app
