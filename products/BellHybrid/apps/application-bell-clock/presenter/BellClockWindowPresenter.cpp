// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellClockWindowPresenter.hpp"

#include <log/log.hpp>

namespace app::bell_clock
{
    BellClockWindowPresenter::BellClockWindowPresenter(std::unique_ptr<AbstractTimeModel> timeModel,
                                                       app::clock::models::DeepRefreshInterval &deepRefreshModel)
        : timeModel{std::move(timeModel)}, deepRefreshModel{deepRefreshModel}
    {}

    void BellClockWindowPresenter::updateTime()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    gui::RefreshModes BellClockWindowPresenter::handleUpdateTimeEvent()
    {
        updateTime();
        return handleCyclicDeepRefresh();
    }

    gui::RefreshModes BellClockWindowPresenter::handleCyclicDeepRefresh()
    {
        static auto refreshCount  = 0;
        gui::RefreshModes refresh = gui::RefreshModes::GUI_REFRESH_FAST;

        const auto deepRefreshPeriod = deepRefreshModel.getValue();
        if (refreshCount >= deepRefreshPeriod) {
            refresh      = gui::RefreshModes::GUI_REFRESH_DEEP;
            refreshCount = 0;
        }
        LOG_ERROR(">>> Refresh (%d/%d) %s", refreshCount, deepRefreshPeriod, magic_enum::enum_name(refresh).data());
        refreshCount++;
        return refresh;
    }

} // namespace app::bell_clock
