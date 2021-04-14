// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundSelectWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/models/SoundsModel.hpp"
#include "application-settings-new/widgets/SettingsSoundItem.hpp"
#include "application-settings-new/data/SoundSelectData.hpp"
#include <gui/widgets/ListView.hpp>
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
                                        style::header::height - 1,
                                        style::listview::body_width_with_scroll,
                                        style::window_height - style::header::height + 1 - style::footer::height +
                                            style::margins::small,
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
