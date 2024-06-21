// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <AppWindow.hpp>
#include <OptionsList.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>

namespace gui
{
    class BellOptionWithDescriptionWindow : public AppWindow, protected OptionsList<ListView>
    {
      public:
        using TokenMap                                 = std::optional<text::RichTextParser::TokenMap>;
        static constexpr auto defaultDescriptionHeight = 175U;

        BellOptionWithDescriptionWindow(app::ApplicationCommon *app, const std::string &name);
        void setListTitle(const std::string &title, const std::string &font = style::window::font::largelight);
        void setListDescription(const std::string &title,
                                TokenMap tokenMap       = {},
                                const std::string &font = style::window::font::verybiglight,
                                unsigned height         = defaultDescriptionHeight);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void buildInterface() override;

      protected:
        BellBaseLayout *body{nullptr};
    };
}; // namespace gui
