// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <unordered_map>

#include "service-cellular/PacketDataTypes.hpp"

#include <at/Result.hpp>

class ServiceCellular;
class DLCChannel;

namespace at
{
    namespace response
    {
        /// Separate, specific AT command for quectel, only for TCP/PDP connection/configuration

        bool parseQICSGP(const at::Result &resp, std::shared_ptr<packet_data::APN::Config> retAPN);
        bool parseQIACT(const at::Result &resp, std::vector<std::shared_ptr<packet_data::APN::Config>> &ret);

    } // namespace response
    namespace query
    {
        std::string prepareQICSGP(std::shared_ptr<packet_data::APN::Config> apn, bool setEmpty = false);
    }
} // namespace at

namespace packet_data
{

    class PDPContext
    {

      private:
        [[maybe_unused]] ServiceCellular &cellularService;
        DLCChannel *channel = nullptr;

      public:
        explicit PDPContext(ServiceCellular &cellularService);
        at::Result::Code setConfiguration(std::shared_ptr<APN::Config> apn, bool setEmpty = false);
        std::shared_ptr<APN::Config> getConfiguration(std::uint8_t contextId);
        at::Result::Code activate(std::uint8_t contextId);
        at::Result::Code deactivate(std::uint8_t contextId);
        std::optional<const std::vector<std::shared_ptr<APN::Config>>> getActive();
    };

    /**
     * @brief wrapping class, transfer data (APN) settings. Responsible for maintaining the state
     * between the modem and the settings
     */
    class PacketData
    {
      private:
        ServiceCellular &cellularService;
        ContextMap contextMap;
        DataTransfer dataTransfer = DataTransfer::On; /// depend on APN settings

      public:
        explicit PacketData(ServiceCellular &cellularService);
        /**
         * @brief load APN settings from JSON string, not call modem func.
         *
         * To synchronize with modem, this function should be call as soon as
         * the modem is ready (in sense of AT commands). Then should be call
         * setupAPNSettings to synchronize modem database with phone database.
         */
        void loadAPNSettings(const std::string &json);

        /**
         * @brief save all APN's JSON string
         */
        std::string saveAPNSettings() const;

        /**
         * @brief setup APN on modem, based on configuration loaded in loadAPNSettings
         * cleanup old configuration
         */
        void setupAPNSettings();

        at::Result::Code updateAPNSettings(std::uint8_t ctxId);

        std::optional<const std::vector<std::shared_ptr<APN::Config>>> getActiveContexts();
        at::Result::Code activateContext(std::uint8_t contextId);
        at::Result::Code deactivateContext(std::uint8_t contextId);

        bool setDataTransfer(DataTransfer dt);
        DataTransfer getDataTransfer() const;
        /**
         * @brief get one APN from phone configuration, connected with ctxid
         */
        std::optional<std::shared_ptr<APN::Config>> getAPN(std::uint8_t ctxid);
        /**
         * @brief get all APNs from phone configuration
         */
        const std::vector<std::shared_ptr<APN::Config>> getAPNs() const;
        /**
         * @brief get first APN with type, from phone configuration
         */
        std::optional<std::shared_ptr<APN::Config>> getAPNFirst(APN::APNType type);
        at::Result::Code setAPN(std::shared_ptr<APN::Config> apn);
        at::Result::Code newAPN(std::shared_ptr<APN::Config> apn, std::uint8_t &newId);
    };

} // namespace packet_data
