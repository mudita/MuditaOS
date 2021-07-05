// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-music-player/data/MusicPlayerStyle.hpp"

#include <ListItem.hpp>
#include <Text.hpp>
#include <TextFixedSize.hpp>
#include <ImageBox.hpp>

namespace gui
{
    class SongItem : public ListItem
    {

      public:
        SongItem(const std::string &authorName, const std::string &songName, const std::string &duration);

      private:
        VBox *vBox                  = nullptr;
        HBox *firstHBox             = nullptr;
        HBox *secondHBox            = nullptr;
        TextFixedSize *authorText   = nullptr;
        TextFixedSize *songText     = nullptr;
        TextFixedSize *durationText = nullptr;
        ImageBox *playedSong        = nullptr;
    };

} /* namespace gui */
