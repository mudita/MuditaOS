#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

using namespace parserFSM;

class DeviceInfoEndpoint : public Endpoint
{

  public:
    DeviceInfoEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "DeviceInfoEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto getDeviceInfo(Context &context) -> bool;
};
