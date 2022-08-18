// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Rect.hpp"

namespace gui
{

    class ListItem : public Rect
    {

      public:
        bool deleteByList = true;
        void accept(GuiVisitor &visitor) override;

        ListItem();
    };

    template <class T>
    class ListItemWithCallbacks : public ListItem
    {
      public:
        std::function<bool()> onEmptyCallback                           = nullptr;
        std::function<bool()> onContentChangedCallback                  = nullptr;
        std::function<bool(std::string &errorMessage)> onVerifyCallback = nullptr;
        std::function<void(std::shared_ptr<T> record)> onSaveCallback   = nullptr;
        std::function<void(std::shared_ptr<T> record)> onLoadCallback   = nullptr;
    };
} /* namespace gui */
