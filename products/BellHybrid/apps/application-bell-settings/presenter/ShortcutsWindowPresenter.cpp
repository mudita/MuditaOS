// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ShortcutsWindowPresenter.hpp"
#include <common/layouts/ShortcutsLayoutProvider.hpp>
#include <common/layouts/ShortcutsLayouts.hpp>

namespace app::bell_settings
{
    ShortcutsWindowPresenter::ShortcutsWindowPresenter(app::ApplicationCommon *app)
    {
        initLayoutOptions();
    }

    std::vector<gui::Item *> ShortcutsWindowPresenter::getLayouts() const
    {
        return layoutOptions;
    }

    gui::Item *ShortcutsWindowPresenter::getFirstLayout() const
    {
        return layoutOptions.empty() ? nullptr : layoutOptions.front();
    }

    bool ShortcutsWindowPresenter::isLastLayout(const gui::Item *layout) const
    {
        return !layoutOptions.empty() && layoutOptions.back() == layout;
    }

    bool ShortcutsWindowPresenter::isOneOfTwoLastLayouts(const gui::Item *layout) const
    {
        return isLastLayout(layout) || (layoutOptions.size() >= 2 && layoutOptions[layoutOptions.size() - 2] == layout);
    }

    void ShortcutsWindowPresenter::initLayoutOptions()
    {
        const auto layoutsList = gui::factory::getLayouts();

        for (auto &layoutEntry : layoutsList) {
            layoutOptions.push_back(layoutEntry()->getLayout());
        }
    }

    bool ShortcutsWindowPresenter::onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout)
    {
        return false;
    }
} // namespace app::bell_settings
