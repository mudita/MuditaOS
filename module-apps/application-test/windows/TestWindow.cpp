// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LocalStyle.hpp"
#include "TestWindow.hpp"
#include "Icon.hpp"
#include "presenters/TestPresenter.hpp"
#include <typeinfo>

namespace gui
{

    TestMainWindow::TestMainWindow(app::ApplicationCommon *app, const std::string &name, TestPresenter &presenter)
        : AppWindow(app, name), presenter(presenter)
    {
        LOG_INFO("test window build!");
        presenter.attachWindow(this);
        buildInterface();
    }

    void TestMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    void TestMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void TestMainWindow::buildInterface()
    {
        using namespace style::window::test;

        AppWindow::buildInterface();

        // some random image added for this button image
        button = new Icon(this, x_position, y_position, width, height, whatever_image, presenter.getTestButtonText());
        button->setEdges(RectangleEdge::All);
        button->setRadius(rounding_radius);
        button->activatedCallback = [this](Item &) -> bool {
            presenter.handleTestButtonPressed();
            return true;
        };

        setFocusItem(button);
    }

    bool TestMainWindow::onInput(const InputEvent &inputEvent)
    {
        // NOTE
        // This is important - window passes input to GUI tree in this handling!
        // without it you won't be able to call activatedCallback from the button!
        auto ret = AppWindow::onInput(inputEvent);
        return ret;
    }

    void TestMainWindow::changeButtonText(const UTF8 &text)
    {
        button->text->setText(text);
    }
} // namespace gui
