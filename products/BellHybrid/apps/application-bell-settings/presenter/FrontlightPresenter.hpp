
// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/FrontlightModel.hpp>
#include "models/FrontlightListItemProvider.hpp"
#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    namespace FrontlightWindowContract
    {
        class View
        {
          public:
            virtual ~View() noexcept = default;
            virtual void exit()      = 0;
        };
    }; // namespace FrontlightWindowContract

    class AbstractFrontlightPresenter : public BasePresenter<FrontlightWindowContract::View>
    {
      public:
        virtual ~AbstractFrontlightPresenter()                                          = default;
        virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
        virtual void eraseProviderData()                                                = 0;
        virtual void saveConfig()                                                       = 0;
        virtual void setBacklight()                                                     = 0;
        virtual void revertConfig()                                                     = 0;
        virtual bool isConfigSaved()                                                    = 0;
    };

    class FrontlightPresenter : public AbstractFrontlightPresenter
    {
      public:
        FrontlightPresenter(std::shared_ptr<FrontlightListItemProvider> &&provider,
                            std::unique_ptr<AbstractFrontlightModel> &&frontlightModel);
        ~FrontlightPresenter();

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void saveConfig() override;
        void setBacklight() override;
        void revertConfig() override;
        bool isConfigSaved() override;

      private:
        std::shared_ptr<FrontlightListItemProvider> provider;
        std::unique_ptr<AbstractFrontlightModel> frontlightModel;
    };
} // namespace app::bell_settings
