// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PopupBlueprintFactory.hpp"

namespace app
{
    void PopupBlueprintFactory::registerBlueprint(gui::popup::ID id, gui::popup::Blueprint &&bp)
    {
        blueprints[id] = bp;
    }

    std::optional<gui::popup::Blueprint> PopupBlueprintFactory::getBlueprint(gui::popup::ID id)
    {
        if (auto blueprint = blueprints.find(id); blueprint != blueprints.end()) {
            return {blueprint->second};
        }
        return std::nullopt;
    }
} // namespace app
