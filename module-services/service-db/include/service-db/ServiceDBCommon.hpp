// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Common/Query.hpp>
#include <module-db/Interface/BaseInterface.hpp>
#include <service-db/DatabaseAgent.hpp>

#include <set>

class ServiceDBCommon : public sys::Service
{
  protected:
    virtual db::Interface *getInterface(db::Interface::Name interface);
    std::set<std::unique_ptr<DatabaseAgent>> databaseAgents;

  public:
    ServiceDBCommon();

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(sys::ServicePowerMode mode) final;

    void sendUpdateNotification(db::Interface::Name interface, db::Query::Type type, std::optional<uint32_t> recordId);
};
