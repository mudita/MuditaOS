// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PacketData.hpp"

#include <optional>
#include <algorithm>
#include <iterator>

#include <response.hpp>
#include <Utils.hpp>
namespace at
{
    namespace response
    {
        bool parseQICSGP(const at::Result &resp, std::shared_ptr<packet_data::APN::Config> retAPN)
        {

            /// AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>)[,<authentication>[,<cdma_pwd>]]]]
            /// +QICSGP: <context_type>,<APN>,<username>,<password>,<authentication>
            /// +QICSGP: 1,"","","",0
            constexpr auto AT_QICSGP = "+QICSGP:";
            if (auto tokens = at::response::getTokensForATCommand(resp, AT_QICSGP); tokens) {

                constexpr int QICSGP_TokensCount = 5; /// Could be depend ? on firmware version, assume that always have
                                                      /// <context_type>,<APN>,<username>,<password>,<authentication>
                if ((*tokens).size() == QICSGP_TokensCount) {

                    retAPN->contextType =
                        static_cast<packet_data::APN::ContextType>(utils::getNumericValue<unsigned int>((*tokens)[0]));
                    retAPN->apn = (*tokens)[1];
                    utils::findAndReplaceAll(retAPN->apn, at::response::StringDelimiter, "");
                    retAPN->username = (*tokens)[2];
                    utils::findAndReplaceAll(retAPN->username, at::response::StringDelimiter, "");
                    retAPN->password = (*tokens)[3];
                    utils::findAndReplaceAll(retAPN->password, at::response::StringDelimiter, "");
                    retAPN->authMethod =
                        static_cast<packet_data::APN::AuthMethod>(utils::getNumericValue<unsigned int>((*tokens)[4]));
                    return true;
                }
            }
            return false;
        }

        bool parseQIACT(const at::Result &resp, std::vector<std::shared_ptr<packet_data::APN::Config>> &ret)
        {

            /**
             * In case of AT+QIACT?
             * +QIACT:1,<context_state>,<context_type>[,<IP_address>]
             * [.....
             * +QIACT:16,<context_state>,<context_type>[,<IP_address>]]
             *
             * also could be empty list
             */
            constexpr int QIACT_TokensCount        = 3;
            constexpr int QIACT_WithIP_TokensCount = 4;

            constexpr auto AT_QIACT = "+QIACT:";
            if (auto mtokens = at::response::getTokensForATResults(resp, AT_QIACT); mtokens) {
                for (const auto &tokens : *mtokens) {

                    if (tokens.size() < QIACT_TokensCount) {
                        continue;
                    }
                    std::shared_ptr<packet_data::APN::Config> retAPN = std::make_shared<packet_data::APN::Config>();
                    retAPN->contextId                                = utils::getNumericValue<std::uint8_t>(tokens[0]);
                    retAPN->state =
                        static_cast<packet_data::APN::ContextState>(utils::getNumericValue<std::uint8_t>(tokens[1]));
                    retAPN->contextType =
                        static_cast<packet_data::APN::ContextType>(utils::getNumericValue<std::uint8_t>(tokens[2]));

                    if (tokens.size() >= QIACT_WithIP_TokensCount) {
                        retAPN->ip = tokens[3];
                        utils::findAndReplaceAll(retAPN->ip, at::response::StringDelimiter, "");
                    }
                    ret.push_back(retAPN);
                }
                return true; /// empty list is also good result
            }
            return false;
        }
    } // namespace response
    namespace query
    {
        std::string prepareQICSGP(std::shared_ptr<packet_data::APN::Config> apn, bool setEmpty)
        {
            /// AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>)[,<authentication>]]]

            if (setEmpty) {
                LOG_DEBUG("Set empty APN");
                return at::factory(at::AT::QICSGP) + "=" + utils::to_string(static_cast<int>(apn->contextId)) + "," +
                       "1,\"\",\"\",\"\",1";
            }

            return at::factory(at::AT::QICSGP) + "=" + utils::to_string(static_cast<int>(apn->contextId)) + "," +
                   utils::to_string(static_cast<int>(apn->contextType)) + ",\"" + apn->apn + "\",\"" + apn->username +
                   "\",\"" + apn->password + "\"," + utils::to_string(static_cast<int>(apn->authMethod));
        }

    } // namespace query
} // namespace at

namespace packet_data
{

    PDPContext::PDPContext(ServiceCellular &cellularService) : cellularService(cellularService)
    {
        channel = cellularService.cmux->get(TS0710::Channel::Commands);
    }

    std::shared_ptr<APN::Config> PDPContext::getConfiguration(std::uint8_t contextId)
    {
        if (channel) {
            auto resp = channel->cmd(at::factory(at::AT::QICSGP) + "=" + utils::to_string(static_cast<int>(contextId)));
            if (resp->getStatusCode() == at::Result::StatusCode::OK) {
                std::shared_ptr<APN::Config> ret = std::make_shared<APN::Config>();
                ret->contextId                   = contextId;
                if (at::response::parseQICSGP(*resp.get(), ret)) {
                    return ret;
                }
            }
        }
        return nullptr;
    }

    at::Result::StatusCode PDPContext::setConfiguration(std::shared_ptr<APN::Config> apn, bool setEmpty)
    {
        if (channel) {
            auto resp = channel->cmd(at::query::prepareQICSGP(apn, setEmpty));
            return resp->getStatusCode();
        }
        return at::Result::StatusCode::ERROR;
    }

    at::Result::StatusCode PDPContext::activate(std::uint8_t contextId)
    {

        if (channel) {
            auto resp = channel->cmd(at::factory(at::AT::QIACT) + "=" + utils::to_string(static_cast<int>(contextId)));

            /**
             * From quectel documentation:
             * 1. Reboot the module if there is no response in 150s.
             * 2. If failed to deactivate the PDP context for 3 times continuously, then reboot the module.
             */
            return resp->getStatusCode();
        }
        return at::Result::StatusCode::ERROR;
    }
    at::Result::StatusCode PDPContext::deactivate(std::uint8_t contextId)
    {
        if (channel) {
            /// this command could generate URC, deactivate
            auto resp =
                channel->cmd(at::factory(at::AT::QIDEACT) + "=" + utils::to_string(static_cast<int>(contextId)));

            return resp->getStatusCode();
        }
        return at::Result::StatusCode::ERROR;
    }

    std::optional<const std::vector<std::shared_ptr<APN::Config>>> PDPContext::getActive()
    {
        if (channel) {
            auto resp = channel->cmd(at::factory(at::AT::QIACT) + "?");

            if (resp->getStatusCode() == at::Result::StatusCode::OK) {
                std::vector<std::shared_ptr<APN::Config>> ret;
                if (at::response::parseQIACT(*resp.get(), ret)) {
                    return ret;
                }
            }
        }
        return std::nullopt;
    }

    PacketData::PacketData(ServiceCellular &cellularService) : cellularService(cellularService){};

    void PacketData::loadAPNSettings()
    {

        std::shared_ptr<APN::Config> apnConfig = std::make_shared<APN::Config>();
        apnConfig->contextId                   = 1;
        apnConfig->apnType                     = APN::APNType::Default;
        apnConfig->apn                         = "internet";
        apnConfig->authMethod                  = APN::AuthMethod::NONE;

        contextMap[apnConfig->contextId] = apnConfig;

        LOG_ERROR("loadAPNSettings");
    }

    void PacketData::saveAPNSettings()
    {
        /// Save in phone memory
    }

    at::Result::StatusCode PacketData::updateAPNSettings(std::uint8_t ctxId)
    {
        LOG_DEBUG("updateAPNSettings %d", ctxId);
        PDPContext pdpCtx(cellularService);
        std::shared_ptr<APN::Config> apnConfig;
        std::shared_ptr<APN::Config> modemApn;
        if ((modemApn = pdpCtx.getConfiguration(ctxId)) && (modemApn != nullptr)) {

            auto phoneApn = contextMap.find(ctxId);

            if (phoneApn != contextMap.end()) {
                LOG_DEBUG("Phone context exists");
                if (dataTransfer == DataTransfer::Off) {
                    /// set null configuration, solution based on lack of quectel documentation
                    return pdpCtx.setConfiguration(phoneApn->second, true);
                }
                else {
                    if (!phoneApn->second->compare(modemApn)) {
                        /// rebuild configuratio
                        LOG_DEBUG("Update modem context %d", ctxId);
                        return pdpCtx.setConfiguration(phoneApn->second);
                    }
                }
            }
            else {
                LOG_ERROR("Phone context not exists");
                if (!modemApn->isEmpty()) {
                    /** update phone configuration base on modem conf (eg. ims)
                     *
                     * Comment from quectel (2020.12):
                     * As I know, only VZW MBN would use IMS on CID 1 to register IMS service directly.
                     * So we usually ask customers to set up data connection on CID3 for VZW sim card.
                     * Regarding other MBN files, the CID 1 shouldn’t be IMS because the CID1 is used
                     * to activate default bearer for LTE network. So we usually ask customers to
                     * configure their own APN on CID1.  With this rule, it’s easy for customer
                     * to configure their APN no matter which MBN file is activated
                     */
                    LOG_ERROR("Update modem context %d", ctxId);
                    return pdpCtx.setConfiguration(modemApn, true);
                }
            }
        }
        return at::Result::StatusCode::OK;
    }
    void PacketData::setupAPNSettings()
    {
        for (std::uint8_t ctxId = MINContextId; ctxId <= MAXContextId; ctxId++) {
            updateAPNSettings(ctxId);
        }
    }

    std::optional<std::shared_ptr<APN::Config>> PacketData::getAPN(std::uint8_t ctxid)
    {
        auto apn = std::find_if(contextMap.begin(), contextMap.end(), [&ctxid](const ContextPair &pair) {
            return pair.second->contextId == ctxid;
        });

        if (apn != contextMap.end()) {
            return apn->second;
        }

        return std::nullopt;
    }

    const std::vector<std::shared_ptr<APN::Config>> PacketData::getAPNs() const
    {
        std::vector<std::shared_ptr<APN::Config>> vconf;
        std::transform(
            contextMap.begin(), contextMap.end(), std::back_inserter(vconf), [](auto &cm) { return cm.second; });
        return vconf;
    }

    std::optional<std::shared_ptr<APN::Config>> PacketData::getAPNFirst(APN::APNType type)
    {

        auto apn = std::find_if(contextMap.begin(), contextMap.end(), [&type](const ContextPair &pair) -> bool {
            return pair.second->apnType == type;
        });

        if (apn != contextMap.end()) {
            return apn->second;
        }

        return std::nullopt;
    }

    at::Result::StatusCode PacketData::setAPN(std::shared_ptr<APN::Config> apn)
    {
        contextMap[apn->contextId] = apn;
        return updateAPNSettings(apn->contextId);
    }

    bool PacketData::setDataTransfer(DataTransfer dt)
    {
        dataTransfer = dt;
        setupAPNSettings();
        return true;
    }
    DataTransfer PacketData::getDataTransfer() const
    {
        return dataTransfer;
    }

    std::optional<const std::vector<std::shared_ptr<APN::Config>>> PacketData::getActiveContexts()
    {
        PDPContext pdpCtx(cellularService);
        return pdpCtx.getActive();
    }
    at::Result::StatusCode PacketData::activateContext(std::uint8_t contextId)
    {
        PDPContext pdpCtx(cellularService);
        return pdpCtx.activate(contextId);
    }
    at::Result::StatusCode PacketData::deactivateContext(std::uint8_t contextId)
    {
        PDPContext pdpCtx(cellularService);
        return pdpCtx.deactivate(contextId);
    }
} // namespace packet_data
