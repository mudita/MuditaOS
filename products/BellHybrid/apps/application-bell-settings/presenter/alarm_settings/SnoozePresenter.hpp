// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <models/alarm_settings/AbstractSnoozeSettingsModel.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class SnoozeListItemProvider;

    class View
    {
      public:
        virtual ~View() noexcept = default;
        virtual void exit()      = 0;
    };

    class AbstractSnoozePresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractSnoozePresenter()                                              = default;
        virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
        virtual void saveData()                                                         = 0;
        virtual void loadData()                                                         = 0;
    };

    class SnoozePresenter : public AbstractSnoozePresenter
    {
      public:
        SnoozePresenter(std::shared_ptr<SnoozeListItemProvider> provider,
                        std::unique_ptr<AbstractSnoozeSettingsModel> snoozeSettingsModel);
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void saveData() override;
        void loadData() override;

      private:
        std::shared_ptr<SnoozeListItemProvider> provider;
        std::unique_ptr<AbstractSnoozeSettingsModel> snoozeSettingsModel;
    };
} // namespace app::bell_settings
