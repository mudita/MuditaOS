// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/models/LayoutModel.hpp>
#include <common/models/TimeModel.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>

#include <vector>
#include <string>
#include <Item.hpp>

namespace app::bell_settings
{
    class LayoutWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<LayoutWindowContract::View>
        {
          public:
            virtual std::vector<gui::Item *> getLayouts() const = 0;
            virtual gui::Item *getSelectedLayout() const        = 0;
            virtual void setLayout(gui::Item *selectedLayout)   = 0;
        };
    };

    class LayoutWindowPresenter : public LayoutWindowContract::Presenter
    {
      private:
        app::ApplicationCommon *app{};
        std::unique_ptr<AbstractLayoutModel> layoutModel;
        std::unique_ptr<AbstractTimeModel> timeModel;
        std::vector<std::pair<gui::Item *, const std::string>> layoutOptions;
        void initLayoutOptions();

      public:
        explicit LayoutWindowPresenter(app::ApplicationCommon *app,
                                       std::unique_ptr<AbstractLayoutModel> &&layoutModel,
                                       std::unique_ptr<AbstractTimeModel> &&timeModel);

        std::vector<gui::Item *> getLayouts() const override;
        gui::Item *getSelectedLayout() const override;
        void setLayout(gui::Item *selectedLayout) override;
    };
} // namespace app::bell_settings
