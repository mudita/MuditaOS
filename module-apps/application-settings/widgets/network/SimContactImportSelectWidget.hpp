// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
                                     const std::function<void(const UTF8 &text)> &navBarTemporaryMode = nullptr,
                                     const std::function<void()> &navBarRestoreFromTemporaryMode      = nullptr);
        bool isChecked();
    };

} /* namespace gui */
