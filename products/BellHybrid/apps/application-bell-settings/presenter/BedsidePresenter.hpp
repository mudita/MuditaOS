// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/BedsideModel.hpp>
#include <ApplicationCommon.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class BedsideListItemProvider;

    namespace BedsideWindowContract
    {
        class View
        {
          public:
            virtual ~View() noexcept    = default;
            virtual auto exit() -> void = 0;
        };

        class Presenter : public BasePresenter<View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            [[nodiscard]] virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual auto saveData() -> void                                                               = 0;
            virtual auto loadData() -> void                                                               = 0;
            virtual auto eraseProviderData() -> void                                                      = 0;
            virtual auto exitWithoutSave() -> void                                                        = 0;
        };
    } // namespace BedsideWindowContract

    class BedsidePresenter : public BedsideWindowContract::Presenter
    {
      public:
        BedsidePresenter(std::shared_ptr<BedsideListItemProvider> &&provider);
        [[nodiscard]] auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto eraseProviderData() -> void override;
        auto exitWithoutSave() -> void override;

      private:
        std::shared_ptr<BedsideListItemProvider> provider;
    };
} // namespace app::bell_settings
