// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>
#include <CheckBoxWithLabel.hpp>

namespace gui
{
    class SimContactImportSelectWidget : public ListItem
    {
      private:
        gui::CheckBoxWithLabel *checkBoxWithLabel = nullptr;

      public:
        SimContactImportSelectWidget(const std::string &contactName,
                                     const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode = nullptr,
                                     const std::function<void()> &bottomBarRestoreFromTemporaryMode      = nullptr);
        bool isChecked();
    };

} /* namespace gui */
