// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Image.hpp>
#include <Label.hpp>
#include <Rect.hpp>

namespace gui
{

    class VBox;
    class HBox;
    class ContactFlagIconWidget;

    class ContactFlagsWidget : public Rect
    {
      public:
        ContactFlagsWidget(Item *parent = nullptr);
        ~ContactFlagsWidget() override = default;
        void setFavourites(bool isEnabled);
        void setSpeedDial(bool isEnabled, unsigned char position = 0);
        void setICE(bool isEnabled);
        void setBlocked(bool isBlocked);

        bool getFavourites()
        {
            return favourites;
        }
        bool getSpeedDial()
        {
            return speedDial;
        }
        bool getICE()
        {
            return ice;
        }
        bool getBlocked()
        {
            return blocked;
        }

      private:
        void initGUIIcons();
        void buildWidget();
        void repositionIcons();
        void hideIfNoFlags();
        bool favourites                       = false;
        bool speedDial                        = false;
        unsigned char speedDialPos            = 0;
        bool ice                              = false;
        bool blocked                          = false;
        HBox *mainBox                         = nullptr;
        ContactFlagIconWidget *favouritesIcon = nullptr;
        ContactFlagIconWidget *speedDialIcon  = nullptr;
        ContactFlagIconWidget *iceIcon        = nullptr;
        ContactFlagIconWidget *blockedIcon    = nullptr;
        static const unsigned char iconsCount;
    };

} // namespace gui
