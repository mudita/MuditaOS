// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsPresenter.hpp"
#include "models/DeepRefreshInterval.hpp"

#include <ApplicationCommon.hpp>
#include <common/widgets/list_items/Fraction.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/NumericWithBar.hpp>
#include <common/LanguageUtils.hpp>
#include <apps-common/InternalModel.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

#include <log/log.hpp>

namespace app::list_items
{
    struct StartDelayFormatter
    {
      public:
        StartDelayFormatter() : none(utils::translate("app_bell_meditation_interval_none"))
        {}
        std::string operator()(std::uint8_t value) const
        {
            return value == 0 ? none : std::to_string(value);
        }

      private:
        std::string none;
    };

    using StartDelaySpinner = gui::U8IntegerSpinnerWithFormatter<StartDelayFormatter>;
    class DeepRefreshInterval : public app::list_items::details::ListItemBase<StartDelaySpinner>
    {
      public:
        DeepRefreshInterval(spinner_type::range &&range,
                            gui::AbstractSettingsModel<spinner_type::value_type> &model,
                            const std::string &topDescription    = "",
                            const std::string &bottomDescription = "")
            : app::list_items::details::ListItemBase<spinner_type>(
                  std::move(range), model, topDescription, bottomDescription)
        {}

      private:
        void control_bottom_description(const spinner_type::value_type &value) final
        {
            bottomText->setVisible(value != 0);
            bottomText->setRichText(utils::language::getCorrectMinutesNumeralForm(value));
        }
    };

} // namespace app::list_items

namespace app::clock
{
    using namespace gui;
    SettingsPresenter::SettingsPresenter(models::DeepRefreshInterval &startDelayModel)
        : startDelayModel{startDelayModel}
    {
        auto startDelay =
            new list_items::DeepRefreshInterval{list_items::DeepRefreshInterval::spinner_type::range{1, 60, 1},
                                                startDelayModel,
                                                "Deep refresh interval",
                                                "minutes"};

        listItemsProvider = std::make_shared<BellListItemProvider>(BellListItemProvider::Items{startDelay});

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

    std::shared_ptr<gui::ListItemProvider> SettingsPresenter::getPagesProvider() const
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
} // namespace app::clock
