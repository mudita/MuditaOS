// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <service-cellular/CellularMessage.hpp>

class AbstractTechnicalInformationRepository
{
  public:
    using OnReadCallback                                       = std::function<void()>;
    virtual ~AbstractTechnicalInformationRepository() noexcept = default;
    virtual void readImei(OnReadCallback callback)             = 0;
    virtual std::string getImei()                              = 0;
};

class TechnicalInformationRepository : public AbstractTechnicalInformationRepository, public app::AsyncCallbackReceiver
{
  public:
    explicit TechnicalInformationRepository(app::Application *application);
    void readImei(OnReadCallback callback) override;
    std::string getImei() override;

  private:
    app::Application *application{};
    std::string imeiStr;
};
