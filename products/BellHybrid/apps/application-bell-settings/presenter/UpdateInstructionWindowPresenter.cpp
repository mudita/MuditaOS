// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "UpdateInstructionWindowPresenter.hpp"
#include <common/layouts/UpdateInstructionLayoutProvider.hpp>
#include <common/layouts/UpdateInstructionLayouts.hpp>

namespace app::bell_settings
{
    UpdateInstructionWindowPresenter::UpdateInstructionWindowPresenter(app::ApplicationCommon *app) : app(app)
    {
        initLayoutOptions();
    }

    std::vector<gui::Item *> UpdateInstructionWindowPresenter::getLayouts() const
    {
        return layoutOptions;
    }

    gui::Item *UpdateInstructionWindowPresenter::getFirstLayout() const
    {
        return layoutOptions.empty() ? nullptr : layoutOptions.front();
    }

    bool UpdateInstructionWindowPresenter::isLastLayout(const gui::Item *layout) const
    {
        return !layoutOptions.empty() && layoutOptions.back() == layout;
    }

    void UpdateInstructionWindowPresenter::initLayoutOptions()
    {
        const auto layoutsList = gui::factory::getUpdateInstructionLayouts();

        for (auto &layoutEntry : layoutsList) {
            layoutOptions.push_back(layoutEntry()->getLayout());
        }
    }

    bool UpdateInstructionWindowPresenter::onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout)
    {
        return false;
    }
} // namespace app::bell_settings
