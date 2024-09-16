// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "OnBoardingUpdateInstructionWindowPresenter.hpp"
#include <BellOnBoardingNames.hpp>
#include <common/layouts/UpdateInstructionLayoutProvider.hpp>
#include <common/layouts/UpdateInstructionLayouts.hpp>
#include <InputEvent.hpp>
#include <keymap/KeyMap.hpp>

namespace app::OnBoarding
{
    OnBoardingUpdateInstructionWindowPresenter::OnBoardingUpdateInstructionWindowPresenter(app::ApplicationCommon *app)
        : app(app)
    {
        initLayoutOptions();
    }

    std::vector<gui::Item *> OnBoardingUpdateInstructionWindowPresenter::getLayouts() const
    {
        return layoutOptions;
    }

    gui::Item *OnBoardingUpdateInstructionWindowPresenter::getFirstLayout() const
    {
        return layoutOptions.empty() ? nullptr : layoutOptions.front();
    }

    bool OnBoardingUpdateInstructionWindowPresenter::isLastLayout(const gui::Item *layout) const
    {
        return !layoutOptions.empty() && layoutOptions.back() == layout;
    }

    void OnBoardingUpdateInstructionWindowPresenter::initLayoutOptions()
    {
        const auto layoutsList = gui::factory::getUpdateInstructionLayouts();

        for (auto &layoutEntry : layoutsList) {
            layoutOptions.push_back(layoutEntry()->getLayout());
        }
    }

    bool OnBoardingUpdateInstructionWindowPresenter::onInput(const gui::InputEvent &inputEvent,
                                                             const gui::Item *currentLayout)
    {
        if (inputEvent.isShortRelease()) {
            const auto key = mapKey(inputEvent.getKeyCode());
            if (key == KeyMap::LightPress) {
                if (isLastLayout(currentLayout)) {
                    app->switchWindow(gui::window::name::finalizeOnBoardingWindow);
                }
                return true;
            }
        }
        return false;
    }
} // namespace app::OnBoarding
