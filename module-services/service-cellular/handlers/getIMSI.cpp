/*
 * getIMSI.cpp
 *
 *  Created on: 26 lut 2020
 *      Author: kuba
 */

#include "../ServiceCellular.hpp"

bool ServiceCellular::getIMSI(std::string &destination, bool fullNumber)
{
    auto ret = cmux->GetChannel("Commands")->cmd(at::AT::CIMI);

    if (ret)
    {
        if (fullNumber)
        {
            destination = ret.response[0];
        }
        else
        {
            try
            {
                destination = ret.response[0].substr(0, 3);
            }
            catch (std::exception &e)
            {
                LOG_ERROR("ServiceCellular::getIMSI exception: %s", e.what());
                return false;
            }
        }
        return true;
    }
    LOG_ERROR("ServiceCellular::getIMSI failed.");
    return false;
}
