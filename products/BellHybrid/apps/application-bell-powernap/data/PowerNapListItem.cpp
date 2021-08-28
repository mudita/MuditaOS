// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapListItem.hpp"
#include <Utils.hpp>
#include <i18n/i18n.hpp>
#include <gui/widgets/Spinner.hpp>
#include "data/PowerNapStyle.hpp"

namespace
{
    inline constexpr auto spinnerMax  = 180U;
    inline constexpr auto spinnerMin  = 1U;
    inline constexpr auto spinnerStep = 1U;

    std::string getTimeUnitName(int currentSpinnerValue)
    {
        using namespace gui::powerNapStyle::listItem;
        const auto isSingular = currentSpinnerValue == 1;
        return utils::translate(isSingular ? timeUnitSingular : timeUnitPlural);
    }

} // namespace
namespace gui
{
    PowerNapListItem::PowerNapListItem() : BellSideListItem(utils::translate("app_bellmain_power_nap"))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        spinner = new Spinner(spinnerMin, spinnerMax, spinnerStep, Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(powerNapStyle::napPeriodFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        bottomDescription = new Label(body->lastBox);
        bottomDescription->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(powerNapStyle::descriptionFont);
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

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
                bottomDescription->setText(getTimeUnitName(spinner->getCurrentValue()));
                return true;
            }
            return false;
        };
    }

    int PowerNapListItem::getSpinnerValue() const noexcept
    {
        return spinner->getCurrentValue();
    }

    void PowerNapListItem::setSpinnerValue(int value)
    {
        spinner->setCurrentValue(value);
        bottomDescription->setText(getTimeUnitName(spinner->getCurrentValue()));
    }

} // namespace gui
