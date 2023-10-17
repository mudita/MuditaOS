// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QMBNManager.hpp"
#include <Utils.hpp>
#include "response.hpp"
#include <at/ATFactory.hpp>
#include <modem/mux/DLCChannel.h>

namespace at
{
    namespace response
    {
        bool parseQMBNCFGlist(const at::Result &resp, std::vector<at::response::qmbncfg::MBNConfig> &ret)
        {

            constexpr auto AT_QMBNCFGlist         = "+QMBNCFG: \"List\"";
            constexpr int QMBNCFGlist_TokensCount = 6;
            if (auto mtokens = at::response::getTokensForATResults(resp, AT_QMBNCFGlist); mtokens) {
                for (const auto &tokens : *mtokens) {
                    if (tokens.size() < QMBNCFGlist_TokensCount) {
                        return false;
                    }
                    at::response::qmbncfg::MBNConfig MBNCfg;
                    /// Omit index parameter
                    MBNCfg.selected  = utils::getNumericValue<unsigned short>(tokens[1]);
                    MBNCfg.activated = utils::getNumericValue<unsigned short>(tokens[2]);

                    MBNCfg.fileName = tokens[3];
                    utils::findAndReplaceAll(MBNCfg.fileName, at::response::StringDelimiter, "");
                    MBNCfg.version = tokens[4];
                    utils::findAndReplaceAll(MBNCfg.version, at::response::StringDelimiter, "");
                    MBNCfg.releaseDate = tokens[5];
                    utils::findAndReplaceAll(MBNCfg.releaseDate, at::response::StringDelimiter, "");

                    ret.push_back(MBNCfg);
                }
                return true; /// empty list no MBN files
            }
            return false;
        }

    } // namespace response
} // namespace at

at::Result::Code QMBNManager::list(std::vector<at::response::qmbncfg::MBNConfig> &ret)
{

    auto command = at::factory(at::AT::QMBNCFG) + "\"List\"";
    auto resp    = channel->cmd(command);
    if (resp.code != at::Result::Code::OK) {
        return resp.code;
    }
    return (at::response::parseQMBNCFGlist(resp, ret)) ? at::Result::Code::OK : at::Result::Code::ERROR;
}

at::Result::Code QMBNManager::select(const std::string &name)
{

    auto command = at::factory(at::AT::QMBNCFG) + "\"Select\",\"" + name + "\"";
    auto resp    = channel->cmd(command);
    return resp.code;
}

at::Result::Code QMBNManager::deactivate()
{
    auto command = at::factory(at::AT::QMBNCFG) + "\"Deactivate\"";
    auto resp    = channel->cmd(command);
    return resp.code;
}
at::Result::Code QMBNManager::setAutoSelect(at::response::qmbncfg::MBNAutoSelect value)
{
    auto command = at::factory(at::AT::QMBNCFG) + "\"autosel\"," + utils::to_string(static_cast<int>(value));
    auto resp    = channel->cmd(command);
    return resp.code;
}

at::Result::Code QMBNManager::writeNVByte(const std::string &nvfile, std::uint8_t byte)
{
    return writeNV(nvfile, utils::byteToHex<std::uint8_t>(byte));
}

at::Result::Code QMBNManager::writeNV(const std::string &nvfile, const std::string &hexvalue)
{
    auto command = at::factory(at::AT::QNVFW) + "\"" + nvfile + "\"," + hexvalue;
    auto resp    = channel->cmd(command);

    return resp.code;
}

at::Result::Code QMBNManager::readNVByte(const std::string &nvfile, std::uint8_t &byte)
{

    std::string buffer;
    auto result = readNV(nvfile, buffer);

    if (result == at::Result::Code::OK) {
        auto bytevec = utils::hexToBytes(buffer);
        if (bytevec.size() != 1)
            return at::Result::Code::ERROR;
        byte = bytevec[0];
    }

    return result;
}

bool parseQNVFR(const at::Result &resp, std::string &ret)
{

    const std::string_view AT_QNVFR = "+QNVFR:";
    if (auto tokens = at::response::getTokensForATCommand(resp, AT_QNVFR); tokens) {
        constexpr int QNVFR_TokensCount = 1;
        if ((*tokens).size() == QNVFR_TokensCount) {
            ret = (*tokens)[0];
            return true;
        }
    }
    return false;
}

at::Result::Code QMBNManager::readNV(const std::string &nvfile, std::string &hexret)
{

    auto command = at::factory(at::AT::QNVFR) + "\"" + nvfile + "\"";
    auto resp    = channel->cmd(command);

    if (resp.code == at::Result::Code::OK) {

        if (!parseQNVFR(resp, hexret)) {
            return at::Result::Code::ERROR;
        }
    }

    return resp.code;
}
