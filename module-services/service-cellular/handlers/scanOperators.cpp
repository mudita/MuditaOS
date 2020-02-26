/*
 * scanOperators.cpp
 *
 *  Created on: 26 lut 2020
 *      Author: kuba
 */

#include "../ServiceCellular.hpp"

std::vector<std::string> ServiceCellular::scanOperators(void)
{
    auto channel = cmux->GetChannel("Commands");
    std::vector<std::string> result;
    if (channel)
    {
        auto resp = channel->cmd("AT+COPS=?\r", 180000, 2);
        if (resp.code == at::Result::Code::OK)
        {
            std::string rawResponse = resp.response[0];
            std::string toErase("+COPS: ");
            auto pos = rawResponse.find(toErase);
            if (pos != std::string::npos)
            {
                rawResponse.erase(pos, toErase.length());
            }
            std::string delimiter("),(");

            auto end = rawResponse.find(delimiter);
            auto begin = 0;
            while (end != std::string::npos)
            {
                auto operatorInfo = rawResponse.substr(begin, end - begin);
                operatorInfo.erase(std::remove(operatorInfo.begin(), operatorInfo.end(), '('), operatorInfo.end());
                operatorInfo.erase(std::remove(operatorInfo.begin(), operatorInfo.end(), ')'), operatorInfo.end());
                operatorInfo.erase(std::remove(operatorInfo.begin(), operatorInfo.end(), ','), operatorInfo.end());
                result.push_back(operatorInfo);
                begin = end;
                end = rawResponse.find(delimiter, end + 1);
            }
        }
    }
    return result;
}
