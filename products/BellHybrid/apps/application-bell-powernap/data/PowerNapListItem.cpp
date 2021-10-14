// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapListItem.hpp"
#include <i18n/i18n.hpp>
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

        createSpinner();
        createBottomDescription();
        registerCallbacks();
    }

    void PowerNapListItem::createSpinner()
    {
        spinner = new UIntegerSpinner(UIntegerSpinner ::Range{spinnerMin, spinnerMax, spinnerStep}, Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(powerNapStyle::napPeriodFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        spinner->onValueChanged = [this](const auto val) { this->onValueChanged(val); };
    }

    void PowerNapListItem::createBottomDescription()
    {
        setupBottomDescription("");
        bottomText->setMaximumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
    }

    void PowerNapListItem::registerCallbacks()
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
                setBottomDescribtionText(getTimeUnitName(spinner->getCurrentValue()));
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
        setBottomDescribtionText(getTimeUnitName(spinner->getCurrentValue()));
        onValueChanged(value);
    }

    void PowerNapListItem::onValueChanged(const std::uint32_t currentValue)
    {
        const auto isMin = currentValue == spinnerMin;
        const auto isMax = currentValue == spinnerMax;
        body->setArrowVisible(BellBaseLayout::Arrow::Left, not isMin);
        body->setArrowVisible(BellBaseLayout::Arrow::Right, not isMax);
    }

} // namespace gui
