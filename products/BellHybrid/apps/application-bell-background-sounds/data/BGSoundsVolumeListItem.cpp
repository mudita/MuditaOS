// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumeListItem.hpp"
#include "BGSoundsStyle.hpp"
#include <Utils.hpp>
#include <i18n/i18n.hpp>
#include <gui/widgets/Spinner.hpp>

namespace
{
    inline constexpr auto spinnerMax  = 10U;
    inline constexpr auto spinnerMin  = 1U;
    inline constexpr auto spinnerStep = 1U;

} // namespace
namespace gui
{
    BGSoundsVolumeListItem::BGSoundsVolumeListItem() : BellSideListItem(utils::translate("app_settings_volume"))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        createSpinner();
        registerCallbacks();
    }

    void BGSoundsVolumeListItem::createSpinner()
    {
        auto onUpdate = [this](int currentValue) {
            const auto isMin = currentValue == spinnerMin;
            const auto isMax = currentValue == spinnerMax;
            body->setArrowVisible(BellBaseLayout::Arrow::Left, not isMin);
            body->setArrowVisible(BellBaseLayout::Arrow::Right, not isMax);
        };

        spinner = new Spinner(spinnerMin, spinnerMax, spinnerStep, Boundaries::Fixed, std::move(onUpdate));
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(bgSoundsStyle::valumeValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);
    }

    void BGSoundsVolumeListItem::registerCallbacks()
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

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }

    int BGSoundsVolumeListItem::getSpinnerValue() const noexcept
    {
        return spinner->getCurrentValue();
    }

    void BGSoundsVolumeListItem::setSpinnerValue(int value)
    {
        spinner->setCurrentValue(value);
    }

} // namespace gui
