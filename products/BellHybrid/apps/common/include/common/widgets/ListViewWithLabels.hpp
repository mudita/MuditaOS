// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <ListViewWithArrows.hpp>
#include <common/widgets/LabelListItem.hpp>
#include <common/models/SongsModel.hpp>

#include <optional>
#include <map>

namespace
{
    constexpr auto maxItemDisplayed{4U};
} // namespace

namespace gui
{
    class ListItemProvider;
    class LabelMarkerItem;

    template <typename T>
    class ListViewWithLabels : public ListViewWithArrows
    {
      public:
        using ItemsType = std::optional<T>;
        // using TypeLabelsMap = std::map<T, std::string>;

        ListViewWithLabels(Item *parent,
                           unsigned int x,
                           unsigned int y,
                           unsigned int w,
                           unsigned int h,
                           std::shared_ptr<ListItemProvider> prov,
                           std::map<T, std::string> labelsPerType)
            : ListViewWithArrows(parent, x, y, w, h, std::move(prov)), typeToLabel(labelsPerType)
        {
            body->dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool { return true; };
        }

        void reset()
        {
            currentMarker.reset();
            previousType.reset();
            currentType.reset();
            ListViewEngine::reset();
        }

      private:
        void addItemsOnPage()
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

        std::size_t getSlotsLeft()
        {
            if (itemsOnPage > maxItemDisplayed) {
                return 0;
            }
            return maxItemDisplayed - itemsOnPage;
        }

        void addLabelMarker(ListItem *item)
        {
            const auto labelListItem = dynamic_cast<gui::LabelListItem<T> *>(item);
            if (labelListItem == nullptr) {
                return;
            };
            previousType = currentType;
            currentType  = labelListItem->getType();

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

                    body->removeWidget(labelListItem);
                    body->addWidget(createMarkerItem(*previousType));
                    updateState(previousType);

                    /* Add item to body even if it won't fit to avoid manual memory
                     * management for item, but apply correction to currentPageSize
                     * if it is not visible. */
                    body->addWidget(labelListItem);

                    if (initialSlotsLeft == 0) {
                        currentPageSize--;
                        itemsOnPage--;
                    }
                }
                else {
                    const auto songsProvider = dynamic_cast<app::SongsModel<T> *>(provider.get());
                    if (songsProvider == nullptr) {
                        break;
                    }
                    const auto nextItemExist = songsProvider->nextRecordExist(getOrderFromDirection());
                    if (!nextItemExist && getSlotsLeft() == 1) {
                        body->addWidget(createMarkerItem(currentType));
                        updateState(currentType);
                    }
                }
                break;
            }
        }

        void updateState(ItemsType newMarker)
        {
            currentMarker = newMarker;
            itemsOnPage++;
            labelAdded = true;
        }

        LabelMarkerItem *createMarkerItem(ItemsType type)
        {
            if (type.has_value()) {
                const auto label = std::string(utils::translate(typeToLabel.at(type.value())));
                return new LabelMarkerItem(label);
            }
            return new LabelMarkerItem(UTF8(""));
        }

        ItemsType currentType{std::nullopt};
        ItemsType previousType{std::nullopt};
        ItemsType currentMarker{std::nullopt};
        std::uint32_t itemsOnPage{0};
        bool labelAdded{false};
        std::map<T, std::string> typeToLabel;
    };

} // namespace gui
