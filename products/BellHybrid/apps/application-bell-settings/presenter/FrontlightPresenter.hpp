
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        virtual void saveChanges()                                                      = 0;
        virtual void revertUnsavedChanges()                                             = 0;
    };

    class FrontlightPresenter : public AbstractFrontlightPresenter
    {
      public:
        FrontlightPresenter(std::shared_ptr<FrontlightListItemProvider> &&provider,
                            std::unique_ptr<AbstractFrontlightModel> &&frontlightModel);
        ~FrontlightPresenter();

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void saveChanges() override;
        void revertUnsavedChanges() override;

      private:
        std::shared_ptr<FrontlightListItemProvider> provider;
        std::unique_ptr<AbstractFrontlightModel> frontlightModel;
    };
} // namespace app::bell_settings
