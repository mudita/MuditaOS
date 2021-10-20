// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeListItem.hpp"
#include "MeditationStyle.hpp"

#include <Utils.hpp>
#include <i18n/i18n.hpp>

namespace
{
    std::string getTimeUnitName(int currentSpinnerValue)
    {
        using namespace app::meditationStyle::icStyle::list;

        if (currentSpinnerValue == 0) {
            return "";
        }
        const auto isSingular = currentSpinnerValue == 1;
        return utils::translate(isSingular ? timeUnitSingular : timeUnitPlural);
    }
} // namespace

namespace gui
{
    IntervalChimeListItem::IntervalChimeListItem(std::vector<std::string> titles)
        : BellSideListItem(utils::translate("app_meditation_interval_chime")), titles{std::move(titles)}
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        createSpinner();
        createBottomDescription();
        registerCallbacks();
    }

    void IntervalChimeListItem::createSpinner()
    {
        spinner = new UTF8Spinner({titles.begin(), titles.end()}, Boundaries::Continuous);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(app::meditationStyle::icStyle::text::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);

        body->getCenterBox()->addWidget(spinner);
    }

    void IntervalChimeListItem::createBottomDescription()
    {
        bottomDescription = new Label(body->lastBox);
        bottomDescription->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(app::meditationStyle::icStyle::minute::font);
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setText(utils::translate("app_meditation_bell_minutes"));
    }

    void IntervalChimeListItem::registerCallbacks()
    {
        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&](Item &) {
            setFocusItem(focus ? body : nullptr);
            if (focus) {
                setFocusItem(body);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool {
            if (body->onInput(inputEvent)) {
                bottomDescription->setText(getTimeUnitName(getSpinnerIndex()));
                return true;
            }
            return false;
        };
    }

    int IntervalChimeListItem::getSpinnerIndex() const
    {
        std::string value = spinner->getCurrentValue();
        int len           = titles.size();
        for (int i = 0; i < len; i++) {
            if (titles.at(i) == value) {
                return i;
            }
        }
        return -1;
    }

    void IntervalChimeListItem::setSpinnerIndex(int index)
    {
        spinner->setCurrentValue(titles[index]);
        bottomDescription->setText(getTimeUnitName(index));
    }

} // namespace gui
