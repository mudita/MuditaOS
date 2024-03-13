// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsListViewItem.hpp"
#include <common/options/BellOptionWindow.hpp>
#include <common/widgets/ListViewWithLabels.hpp>
#include <common/models/SongsModel.hpp>

namespace gui
{
    SongsListViewItem::SongsListViewItem(const std::string &title,
                                         AbstractSettingsModel<UTF8> &settingsModel,
                                         std::shared_ptr<app::SongsModel> songsModel)
        : BellSideListItemWithCallbacks({}, BellBaseLayout::LayoutType::WithoutArrows), settingsModel{settingsModel},
          songsModel{songsModel}
    {
        rearrangeBaseLayout();

        list = new ListViewWithLabels(body, 0, 0, style::window_width, style::window_height, songsModel);
        list->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        list->applySizeRestrictions(style::bell_options_list::w,
                                    style::bell_options_list::h,
                                    style::bell_options_list::outer_layouts_h,
                                    style::bell_options_list::outer_layouts_margin);
        list->setListTitle(title);
        list->setListTitleFont(style::window::font::mediumbiglight); // TODO fix this random font

        focusChangedCallback = [this]([[maybe_unused]] Item &item) {
            if (focus) {
                set_value(focusedRecordPath);
            }
            OnFocusChangedCallback();
            return true;
        };

        auto onListItemActivate = [this](const auto &record) {
            list->setFocusItem(nullptr);
            return true;
        };
        auto onListItemFocusChange = [this](const auto &record) {
            focusedRecordPath = record.fileInfo.path;
            onValueChange(focusedRecordPath);
            return true;
        };

        songsModel->createData(onListItemActivate, onListItemFocusChange);
        list->rebuildList(listview::RebuildType::Full);
        LOG_ERROR("Rebuilding list fully");

        getValue = [this]() {
            this->settingsModel.setValue(focusedRecordPath);
        };
        setValue = [this]() { focusedRecordPath = this->settingsModel.getValue(); };
    }

    auto SongsListViewItem::value() const -> UTF8
    {
        return focusedRecordPath;
    }

    auto SongsListViewItem::set_value(const UTF8 &value) -> void
    {
        const auto selectedItemIndex = this->songsModel->getRecordIndexForPath(value);
        LOG_ERROR("Rebuilding list with index %d for path %s", selectedItemIndex, value.c_str());
        list->rebuildList(listview::RebuildType::OnPageElement, selectedItemIndex);
    }

    auto SongsListViewItem::set_on_value_change_cb(std::function<void(const UTF8 &)> &&cb) -> void
    {
        onValueChange = std::move(cb);
    }

    auto SongsListViewItem::rearrangeBaseLayout() -> void
    {
        /* Hack enabling to add ListViewWithLabels to object of BellSideListItemWithCallback
         * type without rewriting big part of GUI. Be careful while calling methods from
         * BellBaseLayout inside this widget, as they might no longer work because of this! */
        body->erase(body->firstBox);
        body->erase(body->centerBox);
        body->erase(body->lastBox);
    }
} // namespace gui
