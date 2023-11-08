// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingShortcutsWindowPresenter.hpp"
#include <BellOnBoardingNames.hpp>
#include <common/layouts/ShortcutsLayoutProvider.hpp>
#include <common/layouts/ShortcutsLayouts.hpp>
#include <InputEvent.hpp>
#include <keymap/KeyMap.hpp>

namespace app::OnBoarding
{
    OnBoardingShortcutsWindowPresenter::OnBoardingShortcutsWindowPresenter(app::ApplicationCommon *app) : app(app)
    {
        initLayoutOptions();
    }

    std::vector<gui::Item *> OnBoardingShortcutsWindowPresenter::getLayouts() const
    {
        return layoutOptions;
    }

    gui::Item *OnBoardingShortcutsWindowPresenter::getFirstLayout() const
    {
        return layoutOptions.empty() ? nullptr : layoutOptions.front();
    }

    bool OnBoardingShortcutsWindowPresenter::isLastLayout(const gui::Item *layout) const
    {
        return !layoutOptions.empty() && layoutOptions.back() == layout;
    }

    bool OnBoardingShortcutsWindowPresenter::isOneOfTwoLastLayouts(const gui::Item *layout) const
    {
        return isLastLayout(layout) || (layoutOptions.size() >= 2 && layoutOptions[layoutOptions.size() - 2] == layout);
    }

    void OnBoardingShortcutsWindowPresenter::initLayoutOptions()
    {
        const auto layoutsList = gui::factory::getLayouts();

        for (auto &layoutEntry : layoutsList) {
            layoutOptions.push_back(layoutEntry()->getLayout());
        }
    }

    bool OnBoardingShortcutsWindowPresenter::onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout)
    {
        if (inputEvent.isShortRelease()) {
            const auto key = mapKey(inputEvent.getKeyCode());
            if (key == KeyMap::LightPress) {
                if (isLastLayout(currentLayout)) {
                    app->switchWindow(gui::window::name::onBoardingSettingsWindow);
                }
                return true;
            }
        }
        return false;
    }
} // namespace app::OnBoarding
