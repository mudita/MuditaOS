// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMeditation.hpp"
#include "MeditationModel.hpp"
#include "Names.hpp"
#include "Style.hpp"

#include <gui/widgets/ListView.hpp>

using namespace gui;

MeditationModel::MeditationModel(app::Application *app) : application(app)
{}

auto MeditationModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto MeditationModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return style::meditation::itemList::text::Height;
}

void MeditationModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto MeditationModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void MeditationModel::clearData()
{
    list->reset();
    eraseInternalData();
}

PreparationTimeModel::PreparationTimeModel(app::Application *app) : MeditationModel(app)
{}

void PreparationTimeModel::clearData()
{
    itemCurrentlySelected = nullptr;
    MeditationModel::clearData();
}

namespace
{
    using namespace std::chrono_literals;
    std::vector<std::chrono::seconds> preparationTimeRecords{5s, 10s, 30s, 1min, 2min, 5min, 10min, 15min, 30min};
} // namespace

void PreparationTimeModel::createData()
{
    auto app = static_cast<app::ApplicationMeditation *>(application);

    for (auto record : ::preparationTimeRecords) {
        auto item = new PreparationTimeItem(record);
        // check if record was already selected by value or default initialized
        if (app->state->preparationTime == item->getDuration()) {
            item->select(true);
            itemCurrentlySelected = item;
        }

        item->activatedCallback = [this, app, item](gui::Item &) {
            LOG_DEBUG("Selecting meditation time period");
            if (itemCurrentlySelected != nullptr) {
                itemCurrentlySelected->select(false);
            }
            item->select(true);
            itemCurrentlySelected = item;

            app->state->preparationTime = item->getDuration();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        };

        item->deleteByList = false;
        internalData.push_back(item);
    }
}

MeditationOptionsModel::MeditationOptionsModel(app::Application *app) : MeditationModel(app)
{}

void MeditationOptionsModel::createData()
{
    auto app = static_cast<app::ApplicationMeditation *>(application);

    MeditationListItem *item = new OptionItemMeditationCounter(app->state->showCounter);
    item->activatedCallback  = [this, app, item](gui::Item &) {
        LOG_DEBUG("Meditation options: selecting OptionMeditationCounter");
        app->state->showCounter = !app->state->showCounter;
        item->select(app->state->showCounter);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        return true;
    };
    internalData.push_back(item);

    item                    = new OptionItemPreparation();
    item->activatedCallback = [this](gui::Item &) {
        LOG_DEBUG("Meditation options: selecting OptionPreparation");
        application->switchWindow(app::window::name::meditation_preparation);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        return true;
    };
    internalData.push_back(item);

    for (auto item1 : internalData) {
        item1->deleteByList = false;
    }
}
