// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractRelaxationFadeModel.hpp>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class RelaxationListItemProvider;

    class View
    {
      public:
        virtual ~View() noexcept = default;
        virtual void exit()      = 0;
    };

    class AbstractRelaxationPresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractRelaxationPresenter() noexcept                                 = default;
        virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
        virtual void saveData()                                                         = 0;
        virtual void loadData()                                                         = 0;
    };

    class RelaxationWindowPresenter : public AbstractRelaxationPresenter
    {
      private:
        std::shared_ptr<RelaxationListItemProvider> provider;

      public:
        explicit RelaxationWindowPresenter(std::shared_ptr<RelaxationListItemProvider> provider);
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void saveData() override;
        void loadData() override;
    };
} // namespace app::bell_settings
