/*
 * getOwnNumber.cpp
 *
 *  Created on: 26 lut 2020
 *      Author: kuba
 */

#include "../ServiceCellular.hpp"

bool ServiceCellular::getOwnNumber(std::string &destination)
{
    auto ret = cmux->GetChannel("Commands")->cmd(at::AT::CNUM);

    if (ret)
    {
        auto begin = ret.response[0].find(',');
        auto end = ret.response[0].rfind(',');
        if (begin != std::string::npos && end != std::string::npos)
        {
            std::string number;
            try
            {
                number = ret.response[0].substr(begin, end - begin);
            }
            catch (std::exception &e)
            {
                LOG_ERROR("ServiceCellular::getOwnNumber exception: %s", e.what());
                return false;
            }
            number.erase(std::remove(number.begin(), number.end(), '"'), number.end());
            number.erase(std::remove(number.begin(), number.end(), ','), number.end());

            destination = number;
            return true;
        }
    }
    LOG_ERROR("ServiceCellular::getOwnNumber failed.");
    return false;
}
