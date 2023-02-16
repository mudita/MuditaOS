// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundSelectWindow.hpp"

#include <application-settings/data/SoundSelectData.hpp>

#include <ListView.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    SoundSelectWindow::SoundSelectWindow(app::ApplicationCommon *app,
                                         std::string name,
                                         std::shared_ptr<AbstractSoundsModel> model)
        : AppWindow(app, std::move(name)), mSoundsModel{std::move(model)}
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
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        mSoundsList = new gui::ListView(this,
                                        style::window::default_left_margin,
                                        style::window::default_vertical_pos - 1,
                                        style::listview::body_width_with_scroll,
                                        style::window_height - style::window::default_vertical_pos + 1 -
                                            style::nav_bar::height + style::margins::small,
                                        mSoundsModel,
                                        listview::ScrollBarType::Proportional);
        mSoundsList->setBoundaries(Boundaries::Continuous);

        setFocusItem(mSoundsList);
    }

    void SoundSelectWindow::destroyInterface()
    {
        erase();
    }

    void SoundSelectWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
    {
        const auto info = dynamic_cast<SoundSelectData *>(data);
        if (info == nullptr) {
            LOG_ERROR("Null switch data pointer!");
            return;
        }

        setTitle(info->get().windowTitle);
        mSoundsModel->createData(application, info->get().audioModel);
    }

    void SoundSelectWindow::onClose(Window::CloseReason reason)
    {
        if (reason == Window::CloseReason::ApplicationClose) {
            mSoundsModel->clearData();
        }
    }
} // namespace gui
