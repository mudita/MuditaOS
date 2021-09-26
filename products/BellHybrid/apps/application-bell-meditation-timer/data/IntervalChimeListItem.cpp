// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeListItem.hpp"
#include "MeditationStyle.hpp"

#include <Utils.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    IntervalChimeListItem::IntervalChimeListItem(int size, ScroolSpinner::OnGetTitleCallback getTitleCB)
        : BellSideListItem(utils::translate("app_meditation_interval_chime"))
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        createSpinner(size, std::move(getTitleCB));
        createBottomDescription();
        registerCallbacks();
    }

    void IntervalChimeListItem::createSpinner(int size, ScroolSpinner::OnGetTitleCallback getTitleCB)
    {
        auto onUpdate = [this](int currentIndex) {
            if (onIndexChangedCallback != nullptr) {
                onIndexChangedCallback(currentIndex);
            }
        };

        spinner = new ScroolSpinner(size, Boundaries::Fixed, std::move(getTitleCB), std::move(onUpdate));
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(app::meditationStyle::icStyle::text::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
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
                return true;
            }
            return false;
        };
    }

    auto IntervalChimeListItem::setOnIndexChanged(std::function<void(int)> cb) -> void
    {
        onIndexChangedCallback = cb;
    }

    int IntervalChimeListItem::getSpinnerIndex() const noexcept
    {
        return spinner->getCurrentIndex();
    }

    void IntervalChimeListItem::setSpinnerIndex(int Index)
    {
        spinner->setCurrentIndex(Index);
    }

} // namespace gui
