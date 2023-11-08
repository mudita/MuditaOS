// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <vector>
#include <Item.hpp>

namespace gui
{
    class ShortcutsWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public app::BasePresenter<ShortcutsWindowContract::View>
        {
          public:
            virtual std::vector<gui::Item *> getLayouts() const                                     = 0;
            virtual bool isLastLayout(const gui::Item *layout) const                                = 0;
            virtual bool isOneOfTwoLastLayouts(const gui::Item *layout) const                       = 0;
            virtual gui::Item *getFirstLayout() const                                               = 0;
            virtual bool onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout) = 0;
        };
    };
} // namespace gui
