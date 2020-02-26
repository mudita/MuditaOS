/*
 * getNetworkInfo.cpp
 *
 *  Created on: 26 lut 2020
 *      Author: kuba
 */

#include "../ServiceCellular.hpp"

std::vector<std::string> ServiceCellular::getNetworkInfo(void)
{
    std::vector<std::string> data;
    auto channel = cmux->GetChannel("Commands");
    if (channel)
    {
        auto resp = channel->cmd(at::AT::CSQ);
        if (resp.code == at::Result::Code::OK)
        {
            // push back to response message
            std::string ret = resp.response[0];
            std::string toErase = "+CSQ: ";
            auto pos = ret.find(toErase);
            if (pos != std::string::npos)
            {
                ret.erase(pos, toErase.length());
                data.push_back(ret);
            }
            else
            {
                data.push_back("");
            }
        }
        else
        {
            data.push_back("");
        }

        resp = channel->cmd(at::AT::CREG);
        if (resp.code == at::Result::Code::OK)
        {
            std::map<uint32_t, std::string> cregCodes;
            cregCodes.insert(std::pair<uint32_t, std::string>(0, "Not registred"));
            cregCodes.insert(std::pair<uint32_t, std::string>(1, "Registered, home network"));
            cregCodes.insert(std::pair<uint32_t, std::string>(2, "Not registered, searching"));
            cregCodes.insert(std::pair<uint32_t, std::string>(3, "Registration denied"));
            cregCodes.insert(std::pair<uint32_t, std::string>(4, "Unknown"));
            cregCodes.insert(std::pair<uint32_t, std::string>(5, "Registerd, roaming"));

            std::string rawResponse = resp.response[0];
            auto pos = rawResponse.find(',');

            uint32_t cregValue;
            try
            {
                cregValue = std::stoi(rawResponse.substr(pos + 1, 1));
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("ServiceCellular::GetNetworkInfo exception %s", e.what());
                cregValue = UINT32_MAX;
            }
            // push back to response message
            auto commandCode = cregCodes.find(cregValue);
            if (commandCode != cregCodes.end())
            {
                data.push_back(commandCode->second);
            }
            else
            {
                data.push_back("");
            }
        }
        else
        {
            data.push_back("");
        }

        resp = channel->cmd(at::AT::QNWINFO);
        if (resp.code == at::Result::Code::OK)
        {
            auto rawResponse = resp.response[0];
            std::string toErase("+QNWINFO: ");
            auto pos = rawResponse.find(toErase);
            if (pos != std::string::npos)
            {
                rawResponse.erase(pos, toErase.length());
            }
            rawResponse.erase(std::remove(rawResponse.begin(), rawResponse.end(), '\"'), rawResponse.end());
            data.push_back(rawResponse);
        }
        else
        {
            data.push_back("");
        }
    }
    return data;
}
