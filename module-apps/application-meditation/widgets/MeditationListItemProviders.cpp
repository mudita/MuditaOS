#include "MeditationListItemProviders.hpp"
#include "MeditationListItems.hpp"

#include "ListView.hpp"

#include "application-meditation/windows/Names.hpp"
#include "application-meditation/ApplicationMeditation.hpp"
#include "application-meditation/data/Style.hpp"

#include <cassert>
#include <algorithm>

using namespace gui;
uint32_t MeditationListRecord::nextId = 1;

MeditationProvider::MeditationProvider(app::Application *app) : DatabaseModel(app)
{}

bool MeditationProvider::updateRecords(std::vector<MeditationListRecord> newdbRecords)
{
    DatabaseModel::updateRecords(std::move(newdbRecords));
    ListItemProvider::list->onProviderDataUpdate();
    return true;
}

unsigned int MeditationProvider::requestRecordsCount()
{
    return dbRecords.size();
}

unsigned int MeditationProvider::getMinimalItemHeight() const
{
    return style::meditation::itemList::text::Height;
}

void MeditationProvider::requestRecords(const uint32_t offset, const uint32_t limit)
{
    auto dbRecordsCopy = std::vector<MeditationListRecord>();
    auto upperBound    = std::min<uint32_t>(offset + limit, dbRecords.size());
    for (uint32_t i = offset; i < upperBound; i++) {
        dbRecordsCopy.emplace_back(MeditationListRecord(dbRecords[i]));
    }
    updateRecords(std::move(dbRecordsCopy));
}

PrepTimeProvider::PrepTimeProvider(app::Application *app) : MeditationProvider(app)
{
    constexpr auto secondsInMinute = 60;
    dbRecords.clear();
    dbRecords.push_back(PreparationTimeRecord(5));
    dbRecords.push_back(PreparationTimeRecord(10));
    dbRecords.push_back(PreparationTimeRecord(30));
    dbRecords.push_back(PreparationTimeRecord(1 * secondsInMinute));
    dbRecords.push_back(PreparationTimeRecord(2 * secondsInMinute));
    dbRecords.push_back(PreparationTimeRecord(5 * secondsInMinute));
    dbRecords.push_back(PreparationTimeRecord(10 * secondsInMinute));
    dbRecords.push_back(PreparationTimeRecord(15 * secondsInMinute));
    dbRecords.push_back(PreparationTimeRecord(30 * secondsInMinute));
    dbRecords.push_back(PreparationTimeRecord(60 * secondsInMinute));
}

gui::ListItem *PrepTimeProvider::getItem(gui::Order order)
{
    std::shared_ptr<MeditationListRecord> record = getRecord(order);
    if (record == nullptr) {
        return nullptr;
    }

    auto app = static_cast<app::ApplicationMeditation *>(application);

    auto item = new PrepTimeItem(record->getText());
    // check if record was already selected by id or value in case on default initialization
    if (app->state->preparationRecordIndex == record->getId() ||
        app->state->preparationTime == std::chrono::seconds{record->getValue()}) {
        item->select(true);
        itemCurrentlySelected              = item;
        app->state->preparationRecordIndex = record->getId();
    }

    item->activatedCallback = [this, app, item, record](gui::Item &) {
        LOG_DEBUG("Selecting meditation time period");
        if (itemCurrentlySelected != nullptr) {
            itemCurrentlySelected->select(false);
        }
        item->select(true);
        itemCurrentlySelected = item;

        app->state->preparationTime        = std::chrono::seconds{record->getValue()};
        app->state->preparationRecordIndex = record->getId();
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        return true;
    };
    return item;
}

void PrepTimeProvider::requestRecords(const uint32_t offset, const uint32_t limit)
{
    itemCurrentlySelected = nullptr; // new set of ListItems is in use, itemCurrentlySelected ptr should be invalidated
    MeditationProvider::requestRecords(offset, limit);
}

MeditationOptionsProvider::MeditationOptionsProvider(app::Application *app) : MeditationProvider(app)
{
    dbRecords.clear();
    dbRecords.push_back(MeditationOptionRecord(MeditationListRecord::Type::OptionMeditationCounter));
    dbRecords.push_back(MeditationOptionRecord(MeditationListRecord::Type::OptionPreparation));
}

gui::ListItem *MeditationOptionsProvider::getItem(gui::Order order)
{
    std::shared_ptr<MeditationListRecord> record = getRecord(order);
    if (record == nullptr) {
        return nullptr;
    }
    MeditationListItem *item = nullptr;
    if (record->getType() == MeditationListRecord::Type::OptionMeditationCounter) {
        auto app = static_cast<app::ApplicationMeditation *>(application);

        item                    = new OptionItemMeditationCounter(record->getText(), app->state->showCounter);
        item->activatedCallback = [this, app, item](gui::Item &) {
            LOG_DEBUG("Meditation options: selecting OptionMeditationCounter");
            app->state->showCounter = !app->state->showCounter;
            item->select(app->state->showCounter);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
    }
    else if (record->getType() == MeditationListRecord::Type::OptionPreparation) {
        item                    = new OptionItemPreparation(record->getText());
        item->activatedCallback = [this](gui::Item &) {
            LOG_DEBUG("Meditation options: selecting OptionPreparation");
            application->switchWindow(app::window::name::meditation_preparation);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return true;
        };
    }
    return item;
}
