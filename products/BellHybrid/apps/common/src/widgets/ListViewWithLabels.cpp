// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/ListViewWithLabels.hpp>
#include <common/widgets/LabelMarkerItem.hpp>
#include <common/widgets/LabelOptionWithTick.hpp>
#include <common/models/SongsModel.hpp>

namespace
{
    constexpr auto maxItemsOnPage{4U};
}
namespace gui
{
    ListViewWithLabels::ListViewWithLabels(Item *parent,
                                           unsigned int x,
                                           unsigned int y,
                                           unsigned int w,
                                           unsigned int h,
                                           std::shared_ptr<ListItemProvider> prov)
        : ListViewWithArrows(parent, x, y, w, h, std::move(prov))
    {
        body->dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool { return true; };

        updateScrollCallback = [this](ListViewScrollUpdateData data) {
            if (currentPageSize + data.startIndex < data.elementsCount) {
                listOverlay->lastBox->setVisible(true);
            }
            else {
                listOverlay->lastBox->setVisible(false);
            }

            if (data.startIndex == 0 && titleBody) {
                titleBody->setVisible(true);
                arrowTop->setVisible(false);
                listOverlay->firstBox->setVisible(true);
            }
            else if (data.startIndex > 1) {
                if (titleBody) {
                    titleBody->setVisible(false);
                }
                arrowTop->setVisible(true);
                listOverlay->firstBox->setVisible(true);
            }
            else {
                listOverlay->firstBox->setVisible(false);
            }

            listOverlay->resizeItems();
            // Second resize is needed as we need to first apply max size for center box and next extra margins.
            listOverlay->resizeItems();
        };
    }

    void ListViewWithLabels::addLabelItem()
    {
        if (!labelsCount) {
            return;
        }

        const std::int32_t size = direction == listview::Direction::Top ? -currentPageSize : currentPageSize;

        auto position = 0U;
        for (auto i = 0U; i < labelsCount; ++i) {
            // First label always starts from 0
            if (i == 0) {
                position = 0;
            }
            else {
                position += labelFiles[i - 1].second;
            }
            const auto &[labelName, filesCount] = labelFiles[i];
            const auto isSpace                  = (getSlotsLeft() > 0);
            if (((startIndex + size) == position) && (filesCount > 0) && isSpace) {
                // Make sure that hidden item doesn't allow
                // to display label on the next page
                if ((hiddenItemIndex == position) && (position > 0)) {
                    hiddenItemIndex = 0;
                    break;
                }
                body->addWidget(createMarkerItem(labelName));
                itemsOnPage++;
            }
        }
    }

    void ListViewWithLabels::getLabels()
    {
        const auto songsProvider = std::dynamic_pointer_cast<app::SongsModel>(provider);
        if (songsProvider) {
            labelFiles  = songsProvider->getLabelsFilesCount();
            labelsCount = labelFiles.size();
        }
    }

    std::uint32_t ListViewWithLabels::getLabelsCount(unsigned int index)
    {
        auto total  = 0U;
        auto offset = 0U;
        for (const auto &[_, files] : labelFiles) {
            if (index > offset) {
                total++;
            }
            offset += files;
        }
        return total;
    }

    void ListViewWithLabels::addItemsOnPage()
    {
        getLabels();

        const auto rebuildType = lastRebuildRequest.first;
        if ((storedFocusIndex != listview::nPos) && (rebuildType != listview::RebuildType::InPlace)) {
            const auto totalLabels = getLabelsCount(startIndex + storedFocusIndex);
            const auto nextPage    = (totalLabels + storedFocusIndex) / maxItemsOnPage;
            currentFocusIndex      = storedFocusIndex + totalLabels;
            if (nextPage) {
                startIndex += maxItemsOnPage - totalLabels;
                currentFocusIndex %= maxItemsOnPage;
            }
            else if (startIndex) {
                startIndex -= totalLabels;
            }
            else {
                currentFocusIndex -= totalLabels;
            }
            requestNextPage();
            wasSetFocus = true;
        }
        else {
            currentPageSize = 0;
            itemsOnPage     = 0;
            if (wasSetFocus) {
                storedFocusIndex = currentFocusIndex;
                wasSetFocus      = false;
            }
            addItems();
        }
    }

    void ListViewWithLabels::addItems()
    {
        ListItem *item;
        while (true) {
            item = provider->getItem(getOrderFromDirection());
            if (item == nullptr) {
                // Add label if the direction is Top and we are on the first page
                // So we don't get more songs items but we need to add a label
                addLabelItem();
                break;
            }
            addLabelItem();
            // Check if new item fits, if it does - add it
            if (getSlotsLeft() > 0) {
                body->addWidget(item);
                itemsOnPage++;
            }
            else {
                // Add invisible item to list to avoid memory leak
                item->setVisible(false);
                body->addWidget(item);
                // Save the hidden index for calculating label position
                hiddenItemIndex = startIndex;
                if (direction == listview::Direction::Bottom) {
                    hiddenItemIndex += currentPageSize;
                }
                break;
            }
            currentPageSize++;
        }
        recalculateStartIndex();
    }

    LabelMarkerItem *ListViewWithLabels::createMarkerItem(const std::string &label)
    {
        const auto &labelString = UTF8(utils::translate(label));
        return new LabelMarkerItem(labelString);
    }

    std::size_t ListViewWithLabels::getSlotsLeft() const
    {
        if (itemsOnPage > maxItemsOnPage) {
            return 0;
        }
        return maxItemsOnPage - itemsOnPage;
    }
} // namespace gui
