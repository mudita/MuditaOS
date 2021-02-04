// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <ListItem.hpp>
#include <InputEvent.hpp>

#include "Application.hpp"

namespace gui
{
    class ColorTestListItem : public ListItem
    {
      private:
        VBox *vBox        = nullptr;
        Label *colorLabel = nullptr;
        app::Application *app;

      public:
        explicit ColorTestListItem(app::Application *app, const Color color);

        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        [[nodiscard]] uint8_t getColorIntensity();
    };
} /* namespace gui */
