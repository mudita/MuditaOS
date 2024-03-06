// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/ListViewWithLabels.hpp>
//#include "widgets/RelaxationOption.hpp"
#include "../../application-bell-relaxation/model/RelaxationSongsModel.hpp"
#include <common/widgets/LabelListItem.hpp>
#include <TextFixedSize.hpp>

namespace
{
    constexpr auto maxItemDisplayed{4U};
} // namespace

namespace gui
{
    ListViewWithLabels::ListViewWithLabels(Item *parent,
                                           unsigned int x,
                                           unsigned int y,
                                           unsigned int w,
                                           unsigned int h,
                                           std::shared_ptr<ListItemProvider> prov,
                                           const TypeLabelsMap labelsPerType)
        : ListViewWithArrows(parent, x, y, w, h, std::move(prov)), typeToLabel(labelsPerType)
    {
        body->dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool { return true; };
    }

    void ListViewWithLabels::addItemsOnPage()
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
            currentMarker.reset();
        }
    }

    LabelMarkerItem *ListViewWithLabels::createMarkerItem(ItemsType type)
    {
        if (type.has_value()) {
            const auto label = std::string(utils::translate(typeToLabel.at(type.value())));
            return new LabelMarkerItem(label);
        }
        return new LabelMarkerItem(UTF8(""));
    }

    void ListViewWithLabels::addLabelMarker(ListItem *item)
    {
        const auto LabelListItem = dynamic_cast<gui::LabelListItem *>(item);
        if (LabelListItem == nullptr) {
            return;
        };
        previousType = currentType;
        currentType  = LabelListItem->getType();

        switch (direction) {
        case listview::Direction::Bottom:
            if (currentType != previousType && currentType != currentMarker) {
                body->addWidget(createMarkerItem(*currentType));
                updateState(currentType);
            }
            break;

        case listview::Direction::Top:
            if (currentType != previousType && previousType != currentMarker) {
                const auto initialSlotsLeft = getSlotsLeft();

                body->removeWidget(LabelListItem);
                body->addWidget(createMarkerItem(*previousType));
                updateState(previousType);

                /* Add item to body even if it won't fit to avoid manual memory
                 * management for item, but apply correction to currentPageSize
                 * if it is not visible. */
                body->addWidget(LabelListItem);

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

    std::size_t ListViewWithLabels::getSlotsLeft()
    {
        if (itemsOnPage > maxItemDisplayed) {
            return 0;
        }
        return maxItemDisplayed - itemsOnPage;
    }

    void ListViewWithLabels::reset()
    {
        currentMarker.reset();
        previousType.reset();
        currentType.reset();
        ListViewEngine::reset();
    }

    void ListViewWithLabels::updateState(ItemsType newMarker)
    {
        currentMarker = newMarker;
        itemsOnPage++;
        labelAdded = true;
    }

} // namespace gui
