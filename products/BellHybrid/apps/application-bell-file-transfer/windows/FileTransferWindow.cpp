// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FileTransferWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

#include <module-gui/gui/input/InputEvent.hpp>

#include <module-gui/gui/widgets/text/TextFixedSize.hpp>
#include <module-gui/gui/widgets/ThreeBox.hpp>
#include <module-gui/gui/widgets/Image.hpp>

#include <gui/widgets/Icon.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace gui
{
    FileTransferWindow::FileTransferWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_file_transfer::FileTransferPresenter> &&windowPresenter,
        std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void FileTransferWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        navBar->setVisible(false);

        auto icon = new Icon(this,
                             0,
                             0,
                             style::window_width,
                             style::window_height,
                             "big_upload",
                             utils::translate("File transfer in progress."),
                             ImageTypeSpecifier::W_G);
        icon->image->setMargins({0, 122U, 0, 0});
        icon->text->setFont(style::window::font::verybiglight);
        const auto textPadding = icon->text->getPadding();
        icon->text->setPadding(
            {textPadding.left, 30U, textPadding.right, textPadding.bottom});
        icon->text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        icon->resizeItems();

//        presenter->loadData();

//        setFocusItem(listView);
    }


    void FileTransferWindow::rebuild()
    {
        erase();
        buildInterface();
    }
    auto FileTransferWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        return true;
    }

    //    void BellBedtimeWindow::exit()
//    {
//        presenter->saveData();
//        application->switchWindow(
//            gui::window::bell_finished::defaultName,
//            BellFinishedWindowData::Factory::create("circle_success_big",
//                                                    "",
//                                                    "",
//                                                    BellFinishedWindowData::ExitBehaviour::CloseApplication,
//                                                    std::chrono::seconds::zero()));
//    }
} /* namespace gui */
