// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <vector>
#include <Item.hpp>

namespace gui
{
    class UpdateInstructionWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public app::BasePresenter<UpdateInstructionWindowContract::View>
        {
          public:
            virtual std::vector<gui::Item *> getLayouts() const                                     = 0;
            virtual bool isLastLayout(const gui::Item *layout) const                                = 0;
            virtual gui::Item *getFirstLayout() const                                               = 0;
            virtual bool onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout) = 0;
        };
    };
} // namespace gui
