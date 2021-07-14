// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundSelectWindow.hpp"

#include <application-settings/models/apps/SoundsModel.hpp>
#include <application-settings/data/SoundSelectData.hpp>
#include <ListView.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    SoundSelectWindow::SoundSelectWindow(app::Application *app, std::string name)
        : AppWindow(app, name), mSoundsModel{std::make_shared<SoundsModel>()}
    {
        buildInterface();
    }

    void SoundSelectWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void SoundSelectWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        mSoundsList = new gui::ListView(this,
                                        style::window::default_left_margin,
                                        style::window::default_vertical_pos - 1,
                                        style::listview::body_width_with_scroll,
                                        style::window_height - style::window::default_vertical_pos + 1 -
                                            style::footer::height + style::margins::small,
                                        mSoundsModel,
                                        listview::ScrollBarType::Proportional);

        setFocusItem(mSoundsList);
    }

    void SoundSelectWindow::destroyInterface()
    {
        erase();
    }

    void SoundSelectWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
    {
        auto info = dynamic_cast<SoundSelectData *>(data);
        if (info == nullptr) {
            LOG_ERROR("Null switch data pointer!");
            return;
        }

        setTitle(info->get().windowTitle);
        mSoundsModel->createData(application, info->get().audioModel);
    }
} // namespace gui
