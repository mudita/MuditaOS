// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"
#include <memory>

namespace app::meditation
{
    class StatisticsPresenter : public contract::Presenter
    {
      public:
        StatisticsPresenter();
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void loadData() override;
        void saveData() override;
        void eraseProviderData() override;
        void handleEnter() override;

      private:
    };
} // namespace app::meditation
