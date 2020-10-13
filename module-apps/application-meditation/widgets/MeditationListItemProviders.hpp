#pragma once

#include "ListItemProvider.hpp"

#include "MeditationListRecords.hpp"
#include "AppWindow.hpp"
#include "DatabaseModel.hpp"

namespace gui
{
    class PrepTimeItem;
}
namespace gui
{
    class MeditationProvider : public app::DatabaseModel<MeditationListRecord>, public gui::ListItemProvider
    {
      protected:
        explicit MeditationProvider(app::Application *app);
        std::vector<MeditationListRecord> dbRecords;

      public:
        bool updateRecords(std::vector<MeditationListRecord> dbRecords) final;

        // virtual methods for ListViewProvider
        unsigned int requestRecordsCount() final;
        unsigned int getMinimalItemHeight() const final;
        void requestRecords(const uint32_t offset, const uint32_t limit) override;
    };

    class PrepTimeProvider : public MeditationProvider
    {
        PrepTimeItem *itemCurrentlySelected = nullptr;

      public:
        explicit PrepTimeProvider(app::Application *app);

        // virtual methods for ListViewProvider
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;
    };

    class MeditationOptionsProvider : public MeditationProvider
    {
      public:
        explicit MeditationOptionsProvider(app::Application *app);

        // virtual methods for ListViewProvider
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) final;
    };

} // namespace gui
