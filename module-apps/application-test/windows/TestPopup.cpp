// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LocalStyle.hpp"
#include "TestPopup.hpp"
#include "presenters/TestPresenter.hpp"
#include <typeinfo>

namespace gui
{

    TestPopupWindow::TestPopupWindow(app::ApplicationCommon *app, const std::string &name) : AppWindow(app, name)
    {
        LOG_INFO("popup build!");
        buildInterface();
    }

    void TestPopupWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    void TestPopupWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void TestPopupWindow::buildInterface()
    {
        using namespace style::window::test;

        AppWindow::buildInterface();

        // some random image added for this button image
        auto text = new Text(this, x_position, y_position, width, height);
        text->setRichText("to <b>exit</> press right functional<br>or `enter`");
        text->setAlignment(Alignment::Horizontal::Center);
        text->setEdges(RectangleEdge::All);
        text->setRadius(rounding_radius);
        text->activatedCallback = [this](Item &) -> bool {
            getApplication()->returnToPreviousWindow();
            return true;
        };
        setFocusItem(text);
    }

    bool TestPopupWindow::onInput(const InputEvent &inputEvent)
    {
        auto ret = AppWindow::onInput(inputEvent);
        return ret;
    }
} // namespace gui
