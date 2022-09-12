// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MultimediaFilesRecord.hpp>
#include <string>

namespace gui
{
    class ListItem;
}

namespace app::bgSounds::sound_list_item
{
    gui::ListItem *build(const db::multimedia_files::MultimediaFilesRecord &);
    std::uint32_t size();

} // namespace app::bgSounds::sound_list_item
