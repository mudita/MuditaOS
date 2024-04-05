// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusSettingsPresenter.hpp"
#include "data/FocusCommon.hpp"

#include "models/FocusSettingsModel.hpp"

#include <ApplicationCommon.hpp>
#include <apps-common/InternalModel.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/MinutesWithOff.hpp>
#include <common/widgets/list_items/Multiplicity.hpp>

namespace app::focus
{
    using namespace gui;
    SettingsPresenter::SettingsPresenter(models::FocusSettingsModel &focusTimeModel,
                                         models::FocusSettingsModel &focusRepeatsModel,
                                         models::FocusSettingsModel &shortBreakTimeModel)
        : focusTimeModel{focusTimeModel}, focusRepeatsModel{focusRepeatsModel}, shortBreakTimeModel{shortBreakTimeModel}
    {
        auto focusTime = new list_items::Numeric{
            list_items::Numeric::spinner_type::range{
                models::settings::focusTimeMin, models::settings::focusTimeMax, models::settings::focusTimeStep},
            focusTimeModel,
            utils::translate("app_bell_focus_time"),
            utils::translate("common_minutes_lower_genitive")};

        auto focusRepeats = new list_items::Multiplicity{
            list_items::Multiplicity::spinner_type::range{models::settings::focusRepeatsMin,
                                                          models::settings::focusRepeatsMax,
                                                          models::settings::focusRepeatsStep},
            focusRepeatsModel,
            utils::translate("app_bell_focus_repeats"),
            utils::translate("common_multiplicity_many")};

        auto shortBreakTime = new list_items::MinutesWithOff{
            list_items::MinutesWithOff::spinner_type::range{models::settings::shortBreakTimeMin,
                                                            models::settings::shortBreakTimeMax,
                                                            models::settings::shortBreakTimeStep},
            shortBreakTimeModel,
            utils::translate("app_bell_focus_short_break"),
            utils::translate("common_minutes_lower_genitive")};

        listItemsProvider = std::make_shared<BellListItemProvider>(
            BellListItemProvider::Items{focusTime, focusRepeats, shortBreakTime});

        for (auto &item : listItemsProvider->getListItems()) {
            item->setValue();
        }
    }

    void SettingsPresenter::loadData()
    {
        for (auto &item : listItemsProvider->getListItems()) {
            item->setValue();
        }
    }

    void SettingsPresenter::saveData()
    {
        for (auto &item : listItemsProvider->getListItems()) {
            item->getValue();
        }
    }

    auto SettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return listItemsProvider;
    }

    void SettingsPresenter::eraseProviderData()
    {
        listItemsProvider->clearData();
    }

    void SettingsPresenter::exitWithSave()
    {
        saveData();
        eraseProviderData();
    }

    void SettingsPresenter::exitWithoutSave()
    {
        eraseProviderData();
    }
} // namespace app::focus
