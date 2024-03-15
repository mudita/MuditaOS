// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsListViewItem.hpp"
#include <common/options/BellOptionsNavigation.hpp>
#include <common/options/BellOptionWindow.hpp>
#include <common/widgets/ListViewWithLabels.hpp>
#include <common/models/SongsModel.hpp>

namespace gui
{
    SongsListViewItem::SongsListViewItem(const std::string &title,
                                         AbstractSettingsModel<UTF8> &settingsModel,
                                         std::shared_ptr<app::SongsModel> songsModel)
        : BellSideListItemWithCallbacks({}, BellBaseLayout::LayoutType::WithoutArrows), settingsModel{settingsModel},
          songsModel{std::move(songsModel)}
    {
        rearrangeBaseLayout();

        list = new ListViewWithLabels(body, 0, 0, style::window_width, style::window_height, this->songsModel);
        list->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        list->applySizeRestrictions(style::bell_options_list::w,
                                    style::bell_options_list::h,
                                    style::bell_options_list::outer_layouts_h,
                                    style::bell_options_list::outer_layouts_margin);
        list->setListTitle(title);
        list->setListTitleFont(style::bell_sidelist_item::title_font);

        auto storedCallback = list->inputCallback;
        list->inputCallback = [storedCallback](Item &item, const InputEvent &event) {
            return storedCallback(item, invertNavigationDirection(event));
        };

        getValue = [this]() { this->settingsModel.setValue(this->value()); };
        setValue = [this]() { set_value(this->settingsModel.getValue()); };

        auto onListItemActivate = [this]([[maybe_unused]] const db::multimedia_files::MultimediaFilesRecord &record) {
            set_value(record.fileInfo.path);
            list->rebuildList(listview::RebuildType::InPlace);
            return false;
        };
        auto onListItemFocusAcquire = [this](const db::multimedia_files::MultimediaFilesRecord &record) {
            onValueChange(record.fileInfo.path);
            return true;
        };
        auto onOffsetUpdateCallback = [this](std::uint32_t offset) {
            list->rebuildList(listview::RebuildType::OnPageElement, offset);
            return true;
        };

        setValue();
        this->songsModel->createData(std::move(onListItemActivate),
                                     std::move(onListItemFocusAcquire),
                                     this->settingsModel.getValue(),
                                     std::move(onOffsetUpdateCallback));
    }

    auto SongsListViewItem::value() const -> UTF8
    {
        return songsModel->getCurrentlyChosenRecordPath();
    }

    auto SongsListViewItem::set_value(const UTF8 &value) -> void
    {
        songsModel->setCurrentlyChosenRecordPath(value);
    }

    auto SongsListViewItem::set_on_value_change_cb(std::function<void(const UTF8 &)> &&cb) -> void
    {
        onValueChange = std::move(cb);
    }

    auto SongsListViewItem::rearrangeBaseLayout() -> void
    {
        /* Hack enabling adding ListViewWithLabels to object of type BellSideListItemWithCallback
         * without rewriting big part of GUI. Be careful while calling methods from BellBaseLayout
         * inside this widget, as they might no longer work because of this! */
        body->erase(body->firstBox);
        body->erase(body->centerBox);
        body->erase(body->lastBox);
    }
} // namespace gui
