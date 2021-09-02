// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class InformationWindow : public AppWindow
    {
      protected:
        VBox *box = nullptr;

      public:
        explicit InformationWindow(app::Application *app);
        void buildInterface() override;

      private:
        void addAlignedLabelWithValue(BoxLayout *layout, const std::string &labelText, const std::string &valueText);
    };
} // namespace gui
