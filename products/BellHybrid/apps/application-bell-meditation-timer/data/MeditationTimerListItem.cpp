// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerListItem.hpp"
#include "MeditationStyle.hpp"

#include <gui/widgets/Spinner.hpp>

#include <Utils.hpp>
#include <i18n/i18n.hpp>

namespace
{
    inline constexpr auto spinnerMax  = 180U;
    inline constexpr auto spinnerMin  = 1U;
    inline constexpr auto spinnerStep = 1U;
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
        auto onUpdate = [this](int currentValue) {
            if (app::meditationStyle::mtStyle::config::arrow) {
                const auto isMin = currentValue == spinnerMin;
                const auto isMax = currentValue == spinnerMax;
                body->setArrowVisible(BellBaseLayout::Arrow::Left, not isMin);
                body->setArrowVisible(BellBaseLayout::Arrow::Right, not isMax);
            }
            if (onValueChangedCallback != nullptr) {
                onValueChangedCallback(currentValue);
            }
        };

        spinner = new Spinner(spinnerMin, spinnerMax, spinnerStep, Boundaries::Fixed, std::move(onUpdate));
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(app::meditationStyle::mtStyle::text::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);
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
                return true;
            }
            return false;
        };
    }

    auto MeditationTimerListItem::setOnValueChanged(std::function<void(int)> cb) -> void
    {
        onValueChangedCallback = cb;
    }

    int MeditationTimerListItem::getSpinnerValue() const noexcept
    {
        return spinner->getCurrentValue();
    }

    void MeditationTimerListItem::setSpinnerValue(int value)
    {
        spinner->setCurrentValue(value);
    }

} // namespace gui
