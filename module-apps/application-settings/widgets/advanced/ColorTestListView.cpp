// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ColorTestListView.hpp"

#include <ApplicationCommon.hpp>
#include <InputEvent.hpp>

namespace gui
{
    ColorTestListView::ColorTestListView(app::ApplicationCommon *application,
                                         Item *parent,
                                         uint32_t x,
                                         uint32_t y,
                                         uint32_t w,
                                         uint32_t h,
                                         std::shared_ptr<ListItemProvider> prov,
                                         listview::ScrollBarType scrollBarType)
        : ListView(parent, x, y, w, h, prov, scrollBarType), app{application}
    {
        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease()) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_UP) && pageLoaded) {
                auto result = this->requestPreviousPage();
                app->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                return result;
            }
            else if (inputEvent.is(KeyCode::KEY_DOWN) && pageLoaded) {
                auto result = this->requestNextPage();
                app->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                return result;
            }
            else {
                return false;
            }
        };
    }
} // namespace gui
