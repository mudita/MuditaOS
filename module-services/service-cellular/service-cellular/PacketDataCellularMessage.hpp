// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <optional>
#include "CellularMessage.hpp"

#include "PacketDataTypes.hpp"
#include "Result.hpp"

namespace cellular
{

    class GetAPNMessage : public CellularMessage
    {
        std::optional<std::uint8_t> contextId            = std::nullopt;
        std::optional<packet_data::APN::APNType> apnType = std::nullopt;

      public:
        GetAPNMessage() : CellularMessage(Type::PacketData)
        {}
        GetAPNMessage(std::uint8_t contextId) : CellularMessage(Type::PacketData), contextId(contextId)
        {}
        GetAPNMessage(packet_data::APN::APNType apnType) : CellularMessage(Type::PacketData), apnType(apnType)
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

    class SetAPNMessage : public CellularMessage // also as DeleteAPN
    {
        std::shared_ptr<packet_data::APN::Config> apnConfig;

      public:
        SetAPNMessage(std::shared_ptr<packet_data::APN::Config> apnConfig)
            : CellularMessage(Type::PacketData), apnConfig(std::move(apnConfig))
        {}

        [[nodiscard]] std::shared_ptr<packet_data::APN::Config> getAPNConfig() const noexcept
        {
            return apnConfig;
        }
    };

    class NewAPNMessage : public CellularMessage
    {
        std::shared_ptr<packet_data::APN::Config> apnConfig;

      public:
        NewAPNMessage(std::shared_ptr<packet_data::APN::Config> apnConfig)
            : CellularMessage(Type::PacketData), apnConfig(std::move(apnConfig))
        {}

        [[nodiscard]] std::shared_ptr<packet_data::APN::Config> getAPNConfig() const noexcept
        {
            return apnConfig;
        }
    };

    class SetDataTransferMessage : public CellularMessage
    {
        packet_data::DataTransfer dataTransfer;

      public:
        SetDataTransferMessage(packet_data::DataTransfer dataTransfer)
            : CellularMessage(Type::PacketData), dataTransfer(dataTransfer)
        {}
        [[nodiscard]] packet_data::DataTransfer getDataTransfer() const noexcept
        {
            return dataTransfer;
        }
    };

    class GetDataTransferMessage : public CellularMessage
    {
      public:
        GetDataTransferMessage() : CellularMessage(Type::PacketData)
        {}
    };

    class GetActiveContextsMessage : public CellularMessage
    {
      public:
        GetActiveContextsMessage() : CellularMessage(Type::PacketData)
        {}
    };

    class ActivateContextMessage : public CellularMessage
    {
        std::uint8_t contextId;

      public:
        ActivateContextMessage(std::uint8_t contextId) : CellularMessage(Type::PacketData), contextId(contextId)
        {}
        [[nodiscard]] std::uint8_t getContextId() const noexcept
        {
            return contextId;
        }
    };
    class DeactivateContextMessage : public CellularMessage
    {
        std::uint8_t contextId;

      public:
        DeactivateContextMessage(std::uint8_t contextId) : CellularMessage(Type::PacketData), contextId(contextId)
        {}
        [[nodiscard]] std::uint8_t getContextId() const noexcept
        {
            return contextId;
        }
    };

    // Send from Cellular
    class GetAPNResponse : public CellularMessage
    {
        std::vector<std::shared_ptr<packet_data::APN::Config>> apns;

      public:
        GetAPNResponse(std::vector<std::shared_ptr<packet_data::APN::Config>> apns)
            : CellularMessage(Type::PacketData), apns(std::move(apns))
        {}

        [[nodiscard]] const std::vector<std::shared_ptr<packet_data::APN::Config>> &getAPNs() const noexcept
        {
            return apns;
        }
    };

    class ATResponse : public CellularMessage
    {
        at::Result::Code result;

      public:
        ATResponse(at::Result::Code result) : CellularMessage(Type::PacketData), result(result)
        {}

        [[nodiscard]] at::Result::Code getResult() const noexcept
        {
            return result;
        }
    };

    class SetAPNResponse : public ATResponse
    {
      public:
        SetAPNResponse(at::Result::Code result) : ATResponse(result)
        {}
    };

    class NewAPNResponse : public ATResponse
    {
        std::uint8_t contextId;

      public:
        NewAPNResponse(at::Result::Code result, std::uint8_t contextId) : ATResponse(result), contextId(contextId)
        {}
        [[nodiscard]] std::uint8_t getContextId() const noexcept
        {
            return contextId;
        }
    };

    class SetDataTransferResponse : public ATResponse
    {
      public:
        SetDataTransferResponse(at::Result::Code result) : ATResponse(result)
        {}
    };

    class GetDataTransferResponse : public CellularMessage
    {
        packet_data::DataTransfer dataTransfer;

      public:
        GetDataTransferResponse(packet_data::DataTransfer dataTransfer)
            : CellularMessage(Type::PacketData), dataTransfer(dataTransfer)
        {}

        [[nodiscard]] packet_data::DataTransfer getDataTransfer() const noexcept
        {
            return dataTransfer;
        }
    };

    class GetActiveContextsResponse : public CellularMessage
    {
        std::optional<std::vector<std::shared_ptr<packet_data::APN::Config>>> result;

      public:
        GetActiveContextsResponse(std::optional<std::vector<std::shared_ptr<packet_data::APN::Config>>> result)
            : CellularMessage(Type::PacketData)
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

    class ActivateContextResponse : public ATResponse
    {
        std::uint8_t contextId;

      public:
        ActivateContextResponse(at::Result::Code result, std::uint8_t contextId)
            : ATResponse(result), contextId(contextId)
        {}
        [[nodiscard]] std::uint8_t getContextId() const noexcept
        {
            return contextId;
        }
    };
    class DeactivateContextResponse : public ATResponse
    {
        std::uint8_t contextId;
        bool isUrc;

      public:
        DeactivateContextResponse(at::Result::Code result, std::uint8_t contextId, bool isUrc = false)
            : ATResponse(result), contextId(contextId), isUrc(isUrc)
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
} // namespace cellular
