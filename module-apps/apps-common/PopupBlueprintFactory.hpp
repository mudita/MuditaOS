// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WindowsPopupQueue.hpp"
#include <map>

namespace app
{
    class ApplicationCommon;

    class PopupBlueprintFactory
    {
      private:
        std::map<gui::popup::ID, gui::popup::Blueprint> blueprints;

      public:
        void registerBlueprint(gui::popup::ID id, gui::popup::Blueprint &&);
        std::optional<gui::popup::Blueprint> getBlueprint(gui::popup::ID id);
    };
} // namespace app
