// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <layouts/OnBoardingShortcutsLayouts.hpp>

#include <vector>
#include <string>
#include <Item.hpp>

namespace app::OnBoarding
{
    class OnBoardingShortcutsWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<OnBoardingShortcutsWindowContract::View>
        {
          public:
            virtual std::vector<gui::Item *> getLayouts() const      = 0;
            virtual bool isLastLayout(const gui::Item *layout) const = 0;
            virtual gui::Item *getFirstLayout() const                = 0;
        };
    };

    class OnBoardingShortcutsWindowPresenter : public OnBoardingShortcutsWindowContract::Presenter
    {
      private:
        std::vector<gui::Item *> layoutOptions;
        void initLayoutOptions();

      public:
        explicit OnBoardingShortcutsWindowPresenter(app::ApplicationCommon *app);

        std::vector<gui::Item *> getLayouts() const override;
        bool isLastLayout(const gui::Item *layout) const override;
        gui::Item *getFirstLayout() const override;
    };
} // namespace app::OnBoarding
