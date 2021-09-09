// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <ListItem.hpp>
#include <InputEvent.hpp>

namespace gui
{
    class ColorTestListItem : public ListItem
    {
      private:
        VBox *vBox        = nullptr;
        Label *colorLabel = nullptr;
        app::ApplicationCommon *app;

      public:
        explicit ColorTestListItem(app::ApplicationCommon *app, const Color color);

        bool onInput(const InputEvent &inputEvent) override;
        [[nodiscard]] uint8_t getColorIntensity();
    };
} /* namespace gui */
