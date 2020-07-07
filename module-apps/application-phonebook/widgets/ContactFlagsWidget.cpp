#include "ContactFlagsWidget.hpp"

#include "ContactFlagIconWidget.hpp"
#include "ContactFlagsStyle.hpp"

#include <Alignment.hpp>
#include <BoxLayout.hpp>
#include <i18/i18.hpp>
#include <tools/Common.hpp>
#include <Style.hpp>

namespace gui
{

    const unsigned char ContactFlagsWidget::iconsCount = 4;
    ContactFlagsWidget::ContactFlagsWidget(Item *parent)
        : Rect(parent, 0, style::header::height, style::window_width, style::widget::ContatFlas::height)
    {
        favourites = speedDial = ice = true;
        blocked                      = false;
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        initGUIIcons();
        buildWidget();
        repositionIcons();
    }

    ContactFlagsWidget::~ContactFlagsWidget()
    {
        addWidget(favouritesIcon);
        addWidget(iceIcon);
        addWidget(speedDialIcon);
        addWidget(blockedIcon);
    }

    void ContactFlagsWidget::setFavourites(bool isEnabled)
    {
        if (favourites != isEnabled) {
            favourites = isEnabled;
            repositionIcons();
        }
    }

    void ContactFlagsWidget::setSpeedDial(bool isEnabled, unsigned char position)
    {
        if (speedDialPos != position) {
            speedDialPos = position;
            speedDialIcon->setIconNumber(speedDialPos);
        }
        if (speedDial != isEnabled) {
            speedDial = isEnabled;
            repositionIcons();
        }
    }

    void ContactFlagsWidget::setICE(bool isEnabled)
    {
        if (ice != isEnabled) {
            ice = isEnabled;
            repositionIcons();
        }
    }

    void ContactFlagsWidget::setBlocked(bool isBlocked)
    {
        if (blocked != isBlocked) {
            blocked = isBlocked;
            repositionIcons();
        }
    }

    void ContactFlagsWidget::initGUIIcons()
    {
        favouritesIcon = new ContactFlagIconWidget("small_heart", utils::localize.get("FAVOURITES"), this);
        speedDialIcon  = new ContactFlagIconWidget(speedDialPos, utils::localize.get("SPEED DIAL"), this);
        iceIcon        = new ContactFlagIconWidget("ice", utils::localize.get("ICE"), this);
        blockedIcon    = new ContactFlagIconWidget("block", utils::localize.get("BLOCKED"), this);
    }

    void ContactFlagsWidget::buildWidget()
    {
        mainBox = new HBox(this, 0, 0, style::widget::ContatFlas::itemWidth, style::widget::ContatFlas::itemHeight);
        mainBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        mainBox->setArea(
            {0, 0, style::widget::ContatFlas::itemWidth * iconsCount, style::widget::ContatFlas::itemHeight});
        mainBox->addWidget(favouritesIcon);
        mainBox->addWidget(iceIcon);
        mainBox->addWidget(speedDialIcon);
        mainBox->addWidget(blockedIcon);
        center(this, mainBox, gui::Axis::X);
        center(this, mainBox, gui::Axis::Y);
    }

    void ContactFlagsWidget::repositionIcons()
    {
        unsigned int count(0);
        mainBox->removeWidget(blockedIcon);
        mainBox->removeWidget(favouritesIcon);
        mainBox->removeWidget(speedDialIcon);
        mainBox->removeWidget(iceIcon);
        mainBox->setArea({0, 0, style::widget::ContatFlas::itemWidth, style::widget::ContatFlas::itemHeight});

        favouritesIcon->setVisible(false);
        speedDialIcon->setVisible(false);
        iceIcon->setVisible(false);
        blockedIcon->setVisible(false);

        if (blocked) {
            count = 1;
            blockedIcon->setVisible(true);
            mainBox->addWidget(blockedIcon);
        }
        else {
            if (favourites) {
                favouritesIcon->setVisible(true);
                count++;
            }

            if (ice) {
                iceIcon->setVisible(true);
                count++;
            }

            if (speedDial) {
                speedDialIcon->setVisible(true);
                count++;
            }

            // something's fubar in layout box
            // boxlayout have to be resized before adding widgets but
            // first items have to be visible or it will hide some widget randomly
            mainBox->setArea(
                {0, 0, (style::widget::ContatFlas::itemWidth * count), style::widget::ContatFlas::itemHeight});
            if (favourites) {
                mainBox->addWidget(favouritesIcon);
            }

            if (ice) {
                mainBox->addWidget(iceIcon);
            }

            if (speedDial) {
                mainBox->addWidget(speedDialIcon);
            }
        }
        center(this, mainBox, gui::Axis::X);
        center(this, mainBox, gui::Axis::Y);
    }

} // namespace gui
