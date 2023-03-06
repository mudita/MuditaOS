// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellFileTransfer.hpp"
#include "presenter/FileTransferPresenter.hpp"
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class FileTransferWindow : public AppWindow, public app::bell_file_transfer::View
    {
      public:
        FileTransferWindow(app::ApplicationCommon *app,
                          std::unique_ptr<app::bell_file_transfer::FileTransferPresenter> &&windowPresenter,
                          std::string name = "");

        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void rebuild() override;
//        void exit() override;

      private:
        std::unique_ptr<app::bell_file_transfer::FileTransferPresenter> presenter;
    };
} /* namespace gui */