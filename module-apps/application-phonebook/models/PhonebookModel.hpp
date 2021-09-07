// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"
#include "Application.hpp"
#include "Common/Query.hpp"
#include "DatabaseModel.hpp"
#include "Interface/ContactRecord.hpp"
#include "ListItemProvider.hpp"
#include "NotesRecord.hpp"
#include <module-apps/application-phonebook/widgets/PhonebookListView.hpp>

#include <string>

class PhonebookModel : public app::DatabaseModel<ContactRecord>,
                       public gui::ListItemProvider,
                       public app::AsyncCallbackReceiver
{
  private:
    std::string queryFilter;
    std::uint32_t queryGroupFilter;
    std::uint32_t queryDisplayMode;
    std::uint32_t lastRequestedOffset = 0;

  public:
    ContactsMapData letterMap;
    PhonebookModel(app::Application *app,
                   std::string filter        = "",
                   std::uint32_t groupFilter = 0,
                   std::uint32_t displayMode = 0);

    // virtual methods from DatabaseModel
    auto updateRecords(std::vector<ContactRecord> records) -> bool override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    auto requestLetterMap() -> ContactsMapData;

    // virtual methods for ListViewProvider
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;

    auto handleQueryResponse(db::QueryResult *) -> bool;

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getFilter() const -> const std::string &;

    void setDisplayMode(std::uint32_t displayMode);
    auto getDisplayMode() -> std::uint32_t;
    auto getLastRequestedOffset() -> std::uint32_t;
    auto getLabelMarkerDisplayMode(uint32_t posOnList) -> LabelMarkerDisplayMode;

    // onClick callback to register
    std::function<bool(gui::PhonebookItem *item)> messagesSelectCallback = nullptr;
};
