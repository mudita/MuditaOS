// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/application-onboarding/model/EULARepository.hpp>

#include "functional"
#include "string"
#include "BasePresenter.hpp"

using Function = std::function<void()>;

namespace app::onBoarding
{
    class EULALicenseWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<EULALicenseWindowContract::View>
        {
          public:
            ~Presenter() noexcept override = default;

            virtual void acceptEULA()     = 0;
            virtual std::string getEULA() = 0;
        };
    };

    class EULALicenseWindowPresenter : public EULALicenseWindowContract::Presenter
    {
      public:
        explicit EULALicenseWindowPresenter(Function acceptEULAApplication,
                                            std::unique_ptr<AbstractEULARepository> &&eulaRepository);

        void acceptEULA() override;
        std::string getEULA() override;

      private:
        std::function<void()> acceptEULAApp;
        std::unique_ptr<AbstractEULARepository> eulaRepository;
    };
} // namespace app::onBoarding
