// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <common/models/AbstractSettingsModel.hpp>

namespace app
{
    class SongsModel;
}

namespace gui
{
    class ListViewWithLabels;

    class SongsListViewItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        SongsListViewItem(const std::string &description,
                          AbstractSettingsModel<UTF8> &settingsModel,
                          std::shared_ptr<app::SongsModel> songsModel);

        [[nodiscard]] auto value() const -> UTF8;
        auto set_value(const UTF8 &value) -> void;
        auto set_on_value_change_cb(std::function<void(const UTF8 &)> &&cb) -> void;

      private:
        auto rearrangeBaseLayout() -> void;

        gui::AbstractSettingsModel<UTF8> &settingsModel;
        std::shared_ptr<app::SongsModel> songsModel;
        ListViewWithLabels *list{nullptr};
        std::function<void(const UTF8 &)> onValueChange;
        std::string focusedRecordPath;
    };
} // namespace gui
