// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class Image;
    class Label;
    class Text;
    class BluetoothCheckPasskeyWindow : public AppWindow
    {
      public:
        BluetoothCheckPasskeyWindow(app::Application *app);

      private:
        void buildInterface() override;

        Image *image = nullptr;
        Label *label = nullptr;
        Text *text   = nullptr;
    };
} // namespace gui
