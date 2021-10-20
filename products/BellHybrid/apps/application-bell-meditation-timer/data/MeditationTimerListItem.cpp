// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerListItem.hpp"
#include "MeditationStyle.hpp"

#include <log/log.hpp>
#include <Utils.hpp>
#include <i18n/i18n.hpp>

namespace
{
    inline constexpr auto spinnerMax  = 180U;
    inline constexpr auto spinnerMin  = 1U;
    inline constexpr auto spinnerStep = 1U;

    std::string getTimeUnitName(int currentSpinnerValue)
    {
        using namespace app::meditationStyle::mtStyle::list;
        const auto isSingular = currentSpinnerValue == 1;
        return utils::translate(isSingular ? timeUnitSingular : timeUnitPlural);
    }
} // namespace

namespace gui
{
    MeditationTimerListItem::MeditationTimerListItem() : BellSideListItem(utils::translate("app_meditation_title_main"))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        createSpinner();
        createBottomDescription();
        registerCallbacks();
    }

    void MeditationTimerListItem::createSpinner()
    {
        spinner = new UIntegerSpinner(UIntegerSpinner ::Range{spinnerMin, spinnerMax, spinnerStep}, Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(app::meditationStyle::mtStyle::text::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        spinner->onValueChanged = [this](const auto val) { this->onValueChanged(val); };
    }

    void MeditationTimerListItem::createBottomDescription()
    {
        bottomDescription = new Label(body->lastBox);
        bottomDescription->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(app::meditationStyle::mtStyle::minute::font);
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setText(utils::translate("app_meditation_bell_minutes"));
    }

    void MeditationTimerListItem::registerCallbacks()
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
                bottomDescription->setText(getTimeUnitName(spinner->getCurrentValue()));
                return true;
            }
            return false;
        };
    }

    int MeditationTimerListItem::getSpinnerValue() const noexcept
    {
        return spinner->getCurrentValue();
    }

    void MeditationTimerListItem::setSpinnerValue(int value)
    {
        spinner->setCurrentValue(value);
        bottomDescription->setText(getTimeUnitName(value));
    }

    void MeditationTimerListItem::onValueChanged(const std::uint32_t currentValue)
    {
        const auto isMin = currentValue == spinnerMin;
        const auto isMax = currentValue == spinnerMax;
        body->setArrowVisible(BellBaseLayout::Arrow::Left, not isMin);
        body->setArrowVisible(BellBaseLayout::Arrow::Right, not isMax);
    }
} // namespace gui
