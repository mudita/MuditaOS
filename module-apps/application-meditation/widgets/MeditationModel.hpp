// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationListItems.hpp"

#include <Application.hpp>
#include <gui/widgets/ListItemProvider.hpp>
#include <InternalModel.hpp>

namespace gui
{
    class MeditationModel : public app::InternalModel<MeditationListItem *>, public gui::ListItemProvider
    {
      protected:
        app::Application *application = nullptr;
        MeditationModel(app::Application *app);

      public:
        virtual void clearData();
        virtual void createData()
        {}

        [[nodiscard]] auto requestRecordsCount() -> unsigned int final;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int final;
        auto getItem(gui::Order order) -> gui::ListItem * final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;
    };

    class PreparationTimeModel : public MeditationModel
    {
        MeditationListItem *itemCurrentlySelected = nullptr;

      public:
        explicit PreparationTimeModel(app::Application *app);

        void clearData() final;
        void createData() final;
    };

    class MeditationOptionsModel : public MeditationModel
    {
      public:
        explicit MeditationOptionsModel(app::Application *app);

        void createData() final;
    };

} // namespace gui
