// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "WhatsNewFeaturesModel.hpp"
#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class Item;
}

namespace app::whatsnew
{
    class WhatsNewFeaturesContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<WhatsNewFeaturesContract::View>
        {
          public:
            virtual ~Presenter() = default;

            [[nodiscard]] virtual auto getLayouts() const -> std::vector<gui::Item *>      = 0;
            [[nodiscard]] virtual auto isLastLayout(const gui::Item *layout) const -> bool = 0;
            [[nodiscard]] virtual auto getFirstLayout() const -> gui::Item *               = 0;

            virtual auto setCurrentOsVersion() -> void = 0;
        };
    };

    class WhatsNewFeaturesPresenter : public WhatsNewFeaturesContract::Presenter
    {
      public:
        explicit WhatsNewFeaturesPresenter(models::WhatsNewFeaturesModel &model);

        [[nodiscard]] auto getLayouts() const -> std::vector<gui::Item *> override;
        [[nodiscard]] auto isLastLayout(const gui::Item *layout) const -> bool override;
        [[nodiscard]] auto getFirstLayout() const -> gui::Item * override;

        auto setCurrentOsVersion() -> void override;

      private:
        auto createLayouts() -> void;

        std::vector<gui::Item *> layouts;
        models::WhatsNewFeaturesModel &model;
    };
} // namespace app::whatsnew
