// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationMainWindowPresenter.hpp"
#include <common/options/BellOptionWindow.hpp>
namespace gui
{
    class RelaxationMainWindow : public BellOptionWindow, public app::relaxation::RelaxationMainWindowContract::View
    {
        std::unique_ptr<app::relaxation::RelaxationMainWindowContract::Presenter> presenter;

        void setSoundsList(std::vector<db::multimedia_files::MultimediaFilesRecord> soundsTags);
        void buildInterface() override;

        void onActivated(const db::multimedia_files::MultimediaFilesRecord &selectedSound);

      public:
        RelaxationMainWindow(app::ApplicationCommon *app,
                             std::unique_ptr<app::relaxation::RelaxationMainWindowContract::Presenter> &&presenter);
    };
} // namespace gui
