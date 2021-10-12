// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/apps-common/windows/AppWindow.hpp>

class TestWindow : public gui::AppWindow
{
  public:
    explicit TestWindow(const std::string &name) : gui::AppWindow(nullptr, name)
    {}
};
