// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "RelaxationRunningLoopPresenter.hpp"
#include "widgets/RelaxationPlayer.hpp"

#include <common/models/TimeModel.hpp>
#include <gsl/assert>

namespace app::relaxation
{
    RelaxationRunningLoopPresenter::RelaxationRunningLoopPresenter(settings::Settings *settings,
                                                                   AbstractRelaxationPlayer &player,
                                                                   AbstractBatteryModel &battery,
                                                                   std::unique_ptr<AbstractTimeModel> timeModel)
        : settings{settings}, player{player}, batteryModel{battery}, timeModel{std::move(timeModel)}
    {}

    void RelaxationRunningLoopPresenter::activate(const db::multimedia_files::MultimediaFilesRecord &song)
    {
        auto onStartCallback = [this](audio::RetCode retCode) {
            if (retCode != audio::RetCode::Success) {
                getView()->handleError();
            }
        };

        auto onFinishedCallback = [this](AbstractAudioModel::PlaybackFinishStatus status) {
            if (status != AbstractAudioModel::PlaybackFinishStatus::Normal) {
                getView()->handleDeletedFile(); // Deleted file is currently the only error handled by player
            }
        };

        player.start(song.fileInfo.path,
                     AbstractAudioModel::PlaybackMode::Loop,
                     std::move(onStartCallback),
                     std::move(onFinishedCallback));
    }

    void RelaxationRunningLoopPresenter::stop()
    {
        onFinished();
    }

    void RelaxationRunningLoopPresenter::onFinished()
    {
        auto onStopCallback = [](audio::RetCode retCode) {};
        player.stop(std::move(onStopCallback));
    }

    void RelaxationRunningLoopPresenter::pause()
    {
        auto onPauseCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                getView()->onPaused();
            }
        };
        player.pause(std::move(onPauseCallback));
    }

    void RelaxationRunningLoopPresenter::resume()
    {
        auto onResumeCallback = [this](audio::RetCode retCode) {
            if (retCode == audio::RetCode::Success) {
                getView()->resume();
            }
        };
        player.resume(std::move(onResumeCallback));
    }

    void RelaxationRunningLoopPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    bool RelaxationRunningLoopPresenter::isPaused()
    {
        return player.isPaused();
    }

    void RelaxationRunningLoopPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    Store::Battery RelaxationRunningLoopPresenter::getBatteryState() const
    {
        return batteryModel.getLevelState();
    }

    bool RelaxationRunningLoopPresenter::isBatteryCharging(Store::Battery::State state) const
    {
        return batteryModel.isBatteryCharging(state);
    }
} // namespace app::relaxation
