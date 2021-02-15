// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>
#include "BluetoothHelper.hpp"
#include <Service/Service.hpp>

#include <memory>
#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class BluetoothEndpoint : public parserFSM::Endpoint
{
  private:
    std::unique_ptr<parserFSM::BluetoothHelper> helper;

  public:
    BluetoothEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "BluetoothEndpoint";
        helper    = std::make_unique<parserFSM::BluetoothHelper>(ownerServicePtr);
    }
    auto handle(parserFSM::Context &context) -> void override;
};
