// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextImageColorWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <ApplicationCommon.hpp>

namespace gui
{
    TextImageColorWindow::TextImageColorWindow(app::ApplicationCommon *app)
        : AppWindow(app, window::name::status_bar_img_type)
    {
        AppWindow::buildInterface();

        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        setTitle("Image and Text Color test");

        std::string text =
            std::string("<text align='center' color='") + std::to_string(colour) +
            std::string("'>Press <b>left/right arrow</b><br></br> to change colour: |") + std::to_string(colour) +
            std::string("| <br></br><br></br>Press <b>top/bottom arrow </b> <br></br> to change image: |") +
            std::to_string(image_nr) + std::string("|<br></br></text>");

        testIcon = new Icon(this,
                            0,
                            style::window::default_vertical_pos,
                            style::window_width,
                            style::window_height - style::window::default_vertical_pos - style::nav_bar::height,
                            "empty_list_add_test_W_G_1",
                            text);
    }

    bool TextImageColorWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease()) {
            return false;
        }

        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_LEFT) && colour != 0) {
            colour--;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_RIGHT) && colour != 15) {
            colour++;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_UP) && image_nr != 1) {
            image_nr--;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_DOWN) && image_nr != 4) {
            image_nr++;
        }

        std::string text =
            std::string("<text align='center' color='") + std::to_string(colour) +
            std::string("'>Press <b>left/right arrow</b><br></br> to change colour: |") + std::to_string(colour) +
            std::string("| <br></br><br></br>Press <b>top/bottom arrow </b> <br></br> to change image: |") +
            std::to_string(image_nr) + std::string("|<br></br></text>");

        std::string imageName = std::string("empty_list_add_test_W_G_") + std::to_string(image_nr);

        testIcon->text->setRichText(text);
        testIcon->image->set(imageName);

        application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
        return true;
    }
} // namespace gui
