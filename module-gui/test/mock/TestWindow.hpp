// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-gui/gui/widgets/Window.hpp>

namespace gui
{
    class TestWindow : public Window
    {
      public:
        TestWindow(const std::string &name);
        void buildInterface() final;
        void destroyInterface() final;
        void rebuild() final;
    };
} // namespace gui
