// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
                 std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                 std::function<void()> bottomBarRestoreFromTemporaryMode);

        enum class ItemState
        {
            None,
            Playing,
            Paused
        };
        void setState(ItemState state);

      private:
        VBox *vBox                                                   = nullptr;
        HBox *firstHBox                                              = nullptr;
        HBox *secondHBox                                             = nullptr;
        TextFixedSize *authorText                                    = nullptr;
        TextFixedSize *songText                                      = nullptr;
        TextFixedSize *durationText                                  = nullptr;
        Image *playedSong                                            = nullptr;
        ItemState itemState                                          = ItemState::None;
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;
    };

} /* namespace gui */
