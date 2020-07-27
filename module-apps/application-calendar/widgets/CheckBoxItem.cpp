//#include "application-calendar/models/CustomRepeatModel.hpp"
#include "CheckBoxItem.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    CheckBoxItem::CheckBoxItem(const std::string &description,
                               std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                               std::function<void()> bottomBarRestoreFromTemporaryMode)
        : bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, 62);
        setMaximumSize(style::window::default_body_width, 62);

        // setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        hBox = new gui::HBox(this, 0, 0, 400, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(1);

        inputBoxLabel = new gui::Label(hBox, 0, 0, 0, 0);
        inputBoxLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        inputBoxLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        inputBoxLabel->setFont(style::window::font::medium);
        inputBoxLabel->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        tickImage = new gui::Image(hBox, 0, 0, 0, 0);
        tickImage->setVisible(false);
        tickImage->activeItem = false;

        descriptionLabel->setText(description);
        tickImage->set("small_tick");

        /*this->focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_check"));
                }
            }
            else {
                setFocusItem(nullptr);
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };*/
    }

    bool CheckBoxItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        LOG_DEBUG("SIZE: %i,  %i", newDim.w, newDim.h);
        inputBoxLabel->setArea(BoundingBox(0, newDim.h - 50, 55, 50));

        descriptionLabel->setArea(BoundingBox(75, newDim.h - 40, 280, 40));

        tickImage->setArea(BoundingBox(12, newDim.h - 35, 30, 30));

        return true;
    }

    void CheckBoxItem::changeFont()
    {}

    bool CheckBoxItem::onActivated(void *data)
    {
        /// TODO: Center check/uncheck, now it is on the left side.
        tickImage->setVisible(!tickImage->visible);
        if (tickImage->visible) {
            bottomBarTemporaryMode(utils::localize.get("app_phonebook_uncheck"));
        }
        else {
            bottomBarTemporaryMode(utils::localize.get("app_phonebook_check"));
        }
        return true;
    }

} /* namespace gui */
