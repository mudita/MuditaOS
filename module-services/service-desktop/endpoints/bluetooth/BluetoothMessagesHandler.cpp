// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/bluetooth/BluetoothMessagesHandler.hpp>
#include <endpoints/bluetooth/BluetoothEventMessages.hpp>
#include <endpoints/bluetooth/BluetoothHelper.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/DeveloperModeMessage.hpp>
#include <btstack_util.h>

namespace sdesktop::bluetooth
{
    class ResponseEvent : public sdesktop::bluetooth::BluetoothEvent
    {
      public:
        ResponseEvent(json11::Json::object responseBody)
        {
            context.setResponseBody(std::move(responseBody));
        }
    };

} // namespace sdesktop::bluetooth
using namespace sdesktop::bluetooth;
namespace btConstants = sdesktop::endpoints::json::bluetooth;

namespace
{
    json11::Json::object device2json(const Devicei &device)
    {
        LOG_DEBUG("Device: name=%s, address=%s", device.name.data(), bd_addr_to_str(device.address));
        return json11::Json::object{{btConstants::devicesValues::address, std::string(bd_addr_to_str(device.address))},
                                    {btConstants::devicesValues::name, device.name}};
    }
    json11::Json::array devices2json(const std::vector<Devicei> &devices)
    {
        json11::Json::array serialized;
        serialized.reserve(devices.size());
        for (const auto &device : devices) {
            serialized.emplace_back(device2json(device));
        }
        return serialized;
    }
} // namespace

BluetoothMessagesHandler::BluetoothMessagesHandler(sys::Service *ownerService) : ownerService(ownerService)
{}

auto BluetoothMessagesHandler::handle(message::bluetooth::ResponseStatus *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto status = msg->getStatus();
    json11::Json::object responseBody(
        {{btConstants::state,
          json11::Json::object{
              {btConstants::states::power,
               status.state == BluetoothStatus::State::On ? btConstants::on : btConstants::off},
              {btConstants::states::visibility, status.visibility ? btConstants::on : btConstants::off}}}});
    send(std::make_unique<ResponseEvent>(std::move(responseBody)));
    return std::make_shared<sys::ResponseMessage>();
}

auto BluetoothMessagesHandler::handle(message::bluetooth::ResponseBondedDevices *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    LOG_DEBUG("Currently connected device address=%s", msg->getAddressOfConnectedDevice().c_str());
    json11::Json::object responseBody({{btConstants::devices, devices2json(msg->getDevices())},
                                       {btConstants::devicesValues::address, msg->getAddressOfConnectedDevice()}});
    send(std::make_unique<ResponseEvent>(std::move(responseBody)));
    return std::make_shared<sys::ResponseMessage>();
}
auto BluetoothMessagesHandler::handle(message::bluetooth::ResponseVisibleDevices *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    json11::Json::object responseBody({{btConstants::devices, devices2json(msg->getDevices())}});
    send(std::make_unique<ResponseEvent>(std::move(responseBody)));
    return std::make_shared<sys::ResponseMessage>();
}

void BluetoothMessagesHandler::send(std::unique_ptr<ResponseEvent> event)
{
    auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
    ownerService->bus.sendUnicast(std::move(message), service::name::service_desktop);
}
