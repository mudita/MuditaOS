// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
      protected:
        BellBaseLayout *body{};

      public:
        BellOptionWithDescriptionWindow(app::ApplicationCommon *app, const std::string &name);
        void setListTitle(const std::string &title);
        void setListDescription(const std::string &title);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void buildInterface() override;
    };
}; // namespace gui
