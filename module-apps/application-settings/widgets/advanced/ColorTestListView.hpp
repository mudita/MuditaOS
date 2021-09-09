// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListView.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class ColorTestListView : public ListView
    {
      private:
        app::ApplicationCommon *app;

      public:
        ColorTestListView(app::ApplicationCommon *application,
                          Item *parent,
                          uint32_t x,
                          uint32_t y,
                          uint32_t w,
                          uint32_t h,
                          std::shared_ptr<ListItemProvider> prov,
                          listview::ScrollBarType scrollType = listview::ScrollBarType::Proportional);
    };
} // namespace gui
