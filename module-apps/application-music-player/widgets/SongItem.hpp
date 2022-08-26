// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-music-player/data/MusicPlayerStyle.hpp"

#include <ListItem.hpp>
#include <Text.hpp>
#include <TextFixedSize.hpp>
#include <Image.hpp>

namespace gui
{
    class SongItem : public ListItem
    {

      public:
        SongItem(const std::string &authorName,
                 const std::string &songName,
                 const std::string &duration,
                 std::function<void(const UTF8 &)> navBarTemporaryMode,
                 std::function<void()> navBarRestoreFromTemporaryMode);

        enum class ItemState
        {
            None,
            Playing,
            Paused
        };
        void setState(ItemState state);

      private:
        VBox *vBox                                                = nullptr;
        HBox *firstHBox                                           = nullptr;
        HBox *secondHBox                                          = nullptr;
        Label *authorText                                         = nullptr;
        Label *songText                                           = nullptr;
        TextFixedSize *durationText                               = nullptr;
        Image *playedSong                                         = nullptr;
        ItemState itemState                                       = ItemState::None;
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;
    };

} /* namespace gui */
