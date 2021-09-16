// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::bell_settings
{
    class PrewakeUpModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
      public:
        explicit PrewakeUpModel(app::ApplicationCommon *app);

        ~PrewakeUpModel();

        auto clearData() -> void;

        auto saveData() -> void;

        auto loadData() -> void;

        auto createData() -> void;

        void requestRecords(uint32_t offset, uint32_t limit) override;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

      private:
        app::ApplicationCommon *application{};
    };
} // namespace app::bell_settings
