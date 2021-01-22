// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <optional>
#include "CellularMessage.hpp"
#include "MessageType.hpp"

#include "PacketDataTypes.hpp"
#include "Result.hpp"

class CellularGetAPNMessage : public CellularMessage
{
    std::optional<std::uint8_t> contextId            = std::nullopt;
    std::optional<packet_data::APN::APNType> apnType = std::nullopt;

  public:
    CellularGetAPNMessage() : CellularMessage(MessageType::CellularPacketData)
    {}
    CellularGetAPNMessage(std::uint8_t contextId)
        : CellularMessage(MessageType::CellularPacketData), contextId(contextId)
    {}
    CellularGetAPNMessage(packet_data::APN::APNType apnType)
        : CellularMessage(MessageType::CellularPacketData), apnType(apnType)
    {}
    [[nodiscard]] const std::optional<packet_data::APN::APNType> getAPNType() const noexcept
    {
        return apnType;
    }
    [[nodiscard]] const std::optional<std::uint8_t> getContextId() const noexcept
    {
        return contextId;
    }
};

class CellularSetAPNMessage : public CellularMessage // also as DeleteAPN
{
    std::shared_ptr<packet_data::APN::Config> apnConfig;

  public:
    CellularSetAPNMessage(std::shared_ptr<packet_data::APN::Config> apnConfig)
        : CellularMessage(MessageType::CellularPacketData), apnConfig(std::move(apnConfig))
    {}

    [[nodiscard]] std::shared_ptr<packet_data::APN::Config> getAPNConfig() const noexcept
    {
        return apnConfig;
    }
};

class CellularNewAPNMessage : public CellularMessage
{
    std::shared_ptr<packet_data::APN::Config> apnConfig;

  public:
    CellularNewAPNMessage(std::shared_ptr<packet_data::APN::Config> apnConfig)
        : CellularMessage(MessageType::CellularPacketData), apnConfig(std::move(apnConfig))
    {}

    [[nodiscard]] std::shared_ptr<packet_data::APN::Config> getAPNConfig() const noexcept
    {
        return apnConfig;
    }
};

class CellularSetDataTransferMessage : public CellularMessage
{
    packet_data::DataTransfer dataTransfer;

  public:
    CellularSetDataTransferMessage(packet_data::DataTransfer dataTransfer)
        : CellularMessage(MessageType::CellularPacketData), dataTransfer(dataTransfer)
    {}
    [[nodiscard]] packet_data::DataTransfer getDataTransfer() const noexcept
    {
        return dataTransfer;
    }
};

class CellularGetDataTransferMessage : public CellularMessage
{
  public:
    CellularGetDataTransferMessage() : CellularMessage(MessageType::CellularPacketData)
    {}
};

class CellularGetActiveContextsMessage : public CellularMessage
{
  public:
    CellularGetActiveContextsMessage() : CellularMessage(MessageType::CellularPacketData)
    {}
};

class CellularActivateContextMessage : public CellularMessage
{
    std::uint8_t contextId;

  public:
    CellularActivateContextMessage(std::uint8_t contextId)
        : CellularMessage(MessageType::CellularPacketData), contextId(contextId)
    {}
    [[nodiscard]] std::uint8_t getContextId() const noexcept
    {
        return contextId;
    }
};
class CellularDeactivateContextMessage : public CellularMessage
{
    std::uint8_t contextId;

  public:
    CellularDeactivateContextMessage(std::uint8_t contextId)
        : CellularMessage(MessageType::CellularPacketData), contextId(contextId)
    {}
    [[nodiscard]] std::uint8_t getContextId() const noexcept
    {
        return contextId;
    }
};

// Send from Cellular
class CellularGetAPNResponse : public CellularMessage
{
    std::vector<std::shared_ptr<packet_data::APN::Config>> apns;

  public:
    CellularGetAPNResponse(std::vector<std::shared_ptr<packet_data::APN::Config>> apns)
        : CellularMessage(MessageType::CellularPacketData), apns(std::move(apns))
    {}

    [[nodiscard]] const std::vector<std::shared_ptr<packet_data::APN::Config>> &getAPNs() const noexcept
    {
        return apns;
    }
};

class CellularATResponse : public CellularMessage
{
    at::Result::Code result;

  public:
    CellularATResponse(at::Result::Code result) : CellularMessage(MessageType::CellularPacketData), result(result)
    {}

    [[nodiscard]] at::Result::Code getResult() const noexcept
    {
        return result;
    }
};

class CellularSetAPNResponse : public CellularATResponse
{
  public:
    CellularSetAPNResponse(at::Result::Code result) : CellularATResponse(result)
    {}
};

class CellularNewAPNResponse : public CellularATResponse
{
    std::uint8_t contextId;

  public:
    CellularNewAPNResponse(at::Result::Code result, std::uint8_t contextId)
        : CellularATResponse(result), contextId(contextId)
    {}
    [[nodiscard]] std::uint8_t getContextId() const noexcept
    {
        return contextId;
    }
};

class CellularSetDataTransferResponse : public CellularATResponse
{
  public:
    CellularSetDataTransferResponse(at::Result::Code result) : CellularATResponse(result)
    {}
};

class CellularGetDataTransferResponse : public CellularMessage
{
    packet_data::DataTransfer dataTransfer;

  public:
    CellularGetDataTransferResponse(packet_data::DataTransfer dataTransfer)
        : CellularMessage(MessageType::CellularPacketData), dataTransfer(dataTransfer)
    {}

    [[nodiscard]] packet_data::DataTransfer getDataTransfer() const noexcept
    {
        return dataTransfer;
    }
};

class CellularGetActiveContextsResponse : public CellularMessage
{
    std::optional<std::vector<std::shared_ptr<packet_data::APN::Config>>> result;

  public:
    CellularGetActiveContextsResponse(std::optional<std::vector<std::shared_ptr<packet_data::APN::Config>>> result)
        : CellularMessage(MessageType::CellularPacketData)
    {
        if (result) {
            result = std::move(*result);
        }
        else {
            result = std::nullopt;
        }
    }

    [[nodiscard]] std::optional<const std::vector<std::shared_ptr<packet_data::APN::Config>>> getActive()
    {
        return result;
    }
};

class CellularActivateContextResponse : public CellularATResponse
{
    std::uint8_t contextId;

  public:
    CellularActivateContextResponse(at::Result::Code result, std::uint8_t contextId)
        : CellularATResponse(result), contextId(contextId)
    {}
    [[nodiscard]] std::uint8_t getContextId() const noexcept
    {
        return contextId;
    }
};
class CellularDeactivateContextResponse : public CellularATResponse
{
    std::uint8_t contextId;
    bool isUrc;

  public:
    CellularDeactivateContextResponse(at::Result::Code result, std::uint8_t contextId, bool isUrc = false)
        : CellularATResponse(result), contextId(contextId), isUrc(isUrc)
    {}

    [[nodiscard]] std::uint8_t getContextId() const noexcept
    {
        return contextId;
    }
    /**
     * @breif inform that message come from URC (eg. because call AT+CFUN=0, reset by BTS, ...)
     * @return
     */
    [[nodiscard]] bool isURC()
    {
        return isUrc;
    }
};
