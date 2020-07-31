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
        SongItem(std::string authorName, std::string songName, std::string duration);

        ~SongItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox                  = nullptr;
        HBox *firstHBox             = nullptr;
        HBox *secondHBox            = nullptr;
        TextFixedSize *authorText   = nullptr;
        TextFixedSize *songText     = nullptr;
        TextFixedSize *durationText = nullptr;
        ImageBox *playedSong        = nullptr;
    };

} /* namespace gui */
