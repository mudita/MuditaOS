// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationListView.hpp"
#include "widgets/RelaxationOption.hpp"
#include "model/RelaxationSongsModel.hpp"
#include "RelaxationItem.hpp"
#include <TextFixedSize.hpp>

namespace
{

    constexpr auto maxItemDisplayed{4U};

    const std::map<app::relaxation::MusicType, std::string> typeToLabel{
        {app::relaxation::MusicType::Relaxation, "app_bell_relaxation_sounds"},
        {app::relaxation::MusicType::ColorsOfNoise, "app_bell_relaxation_noises"},
        {app::relaxation::MusicType::User, "app_bell_relaxation_uploaded_sounds"}};

    gui::RelaxationMarkerItem *createMarkerItem(app::relaxation::MusicType musicType)
    {
        const auto label = UTF8(utils::translate(typeToLabel.at(musicType)));
        return new gui::RelaxationMarkerItem(label);
    }

} // namespace

namespace gui
{
    RelaxationListView::RelaxationListView(Item *parent,
                                           unsigned int x,
                                           unsigned int y,
                                           unsigned int w,
                                           unsigned int h,
                                           std::shared_ptr<ListItemProvider> prov)
        : ListViewWithArrows(parent, x, y, w, h, std::move(prov))
    {
        body->dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool { return true; };
    }

    void RelaxationListView::addItemsOnPage()
    {
        currentPageSize = 0;
        itemsOnPage     = 0;
        labelAdded      = false;

        ListItem *item = nullptr;
        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {
            /* If direction is top-to-bottom, add label mark before adding relaxation item. */
            if (direction == listview::Direction::Bottom) {
                addLabelMarker(item);
            }
            /* Check if new item fits, if it does - add it, if not - handle label insertion
             * case for bottom-to-top navigation direction. */
            if (getSlotsLeft() > 0) {
                body->addWidget(item);
                itemsOnPage++;
            }
            else {
                // Add invisible item to list to avoid memory leak
                item->setVisible(false);
                body->addWidget(item);
                break;
            }
            /* If direction is bottom-to-top, add label mark after adding relaxation item. */
            if (direction == listview::Direction::Top) {
                addLabelMarker(item);
            }
            currentPageSize++;
        }

        recalculateStartIndex();

        if (!labelAdded) {
            currentMarker = MusicType::Undefined;
        }
    }

    void RelaxationListView::addLabelMarker(ListItem *item)
    {
        const auto relaxationItem = dynamic_cast<gui::RelaxationItem *>(item);
        if (relaxationItem == nullptr) {
            return;
        };
        previousType = currentType;
        currentType  = relaxationItem->getMusicType();

        switch (direction) {
        case listview::Direction::Bottom:
            if (currentType != previousType && currentType != currentMarker) {
                body->addWidget(createMarkerItem(currentType));
                updateState(currentType);
            }
            break;

        case listview::Direction::Top:
            if (currentType != previousType && previousType != currentMarker) {
                const auto initialSlotsLeft = getSlotsLeft();

                body->removeWidget(relaxationItem);
                body->addWidget(createMarkerItem(previousType));
                updateState(previousType);

                /* Add item to body even if it won't fit to avoid manual memory
                 * management for item, but apply correction to currentPageSize
                 * if it is not visible. */
                body->addWidget(relaxationItem);

                if (initialSlotsLeft == 0) {
                    currentPageSize--;
                    itemsOnPage--;
                }
            }
            else {
                const auto relaxationProvider = dynamic_cast<app::relaxation::RelaxationSongsModel *>(provider.get());
                if (relaxationProvider == nullptr) {
                    break;
                }
                const auto nextItemExist = relaxationProvider->nextRecordExist(getOrderFromDirection());
                if (!nextItemExist && getSlotsLeft() == 1) {
                    body->addWidget(createMarkerItem(currentType));
                    updateState(currentType);
                }
            }
            break;
        }
    }

    std::size_t RelaxationListView::getSlotsLeft()
    {
        if (itemsOnPage > maxItemDisplayed) {
            return 0;
        }
        return maxItemDisplayed - itemsOnPage;
    }

    void RelaxationListView::reset()
    {
        currentMarker = MusicType::Undefined;
        previousType  = MusicType::Undefined;
        currentType   = MusicType::Undefined;
        ListViewEngine::reset();
    }

    void RelaxationListView::updateState(RelaxationListView::MusicType newMarker)
    {
        currentMarker = newMarker;
        itemsOnPage++;
        labelAdded = true;
    }

} // namespace gui
