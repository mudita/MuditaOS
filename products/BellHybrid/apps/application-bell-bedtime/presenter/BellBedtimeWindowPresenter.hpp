// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <apps-common/BasePresenter.hpp>
#include "common/models/AbstractBedtimeModel.hpp"

namespace app
{
    class ApplicationCommon;
}

namespace settings
{
    class Settings;
}

namespace gui
{
    class AppWindow;
    class ListItemProvider;
} // namespace gui

namespace app::bell_bedtime
{
    class BedtimeListItemProvider;

    class View
    {
      public:
        virtual ~View() noexcept = default;
        virtual void exit()      = 0;
    };

    class AbstractBedtimePresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractBedtimePresenter() noexcept                                    = default;
        virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
        virtual void saveData()                                                         = 0;
        virtual void loadData()                                                         = 0;
    };

    class BellBedtimeWindowPresenter : public AbstractBedtimePresenter
    {
      private:
        std::shared_ptr<BedtimeListItemProvider> provider;
        std::unique_ptr<AbstractBedtimeModel> settings;

      public:
        explicit BellBedtimeWindowPresenter(std::shared_ptr<BedtimeListItemProvider> provider);
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void saveData() override;
        void loadData() override;
    };
} // namespace app::bell_bedtime
