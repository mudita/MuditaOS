// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcFactory.hpp>

#include <UrcCreg.hpp>
#include <UrcCtze.hpp>
#include <UrcCusd.hpp>
#include <UrcQind.hpp>
#include <UrcCmti.hpp>
#include <UrcClip.hpp>
#include <UrcPoweredDown.hpp>
#include <UrcResponse.hpp>

using namespace at::urc;

std::unique_ptr<Urc> UrcFactory::Create(const std::string &urcMessage)
{
    if (urcMessage.empty()) {
        return std::make_unique<Urc>(std::string());
    }

    const char headDelimiter = ':';
    auto it                  = std::find(urcMessage.begin(), urcMessage.end(), headDelimiter);
    std::string head         = std::string(urcMessage.begin(), it);
    std::string body         = std::string(it == urcMessage.end() ? urcMessage.begin() : it + 1, urcMessage.end());

    if (Ctze::isURC(head)) {
        return std::make_unique<Ctze>(body);
    }
    else if (Creg::isURC(head)) {
        return std::make_unique<Creg>(body);
    }
    else if (Qind::isURC(head)) {
        return std::make_unique<Qind>(body);
    }
    else if (Cusd::isURC(head)) {
        return std::make_unique<Cusd>(body);
    }
    else if (Cmti::isURC(head)) {
        return std::make_unique<Cmti>(body);
    }
    else if (Clip::isURC(head)) {
        return std::make_unique<Clip>(body);
    }
    else if (PoweredDown::isURC(head)) {
        return std::make_unique<PoweredDown>(body);
    }
    else if (auto type = UrcResponse::isURC(head)) {
        return std::make_unique<UrcResponse>(type.value());
    }

    return std::make_unique<Urc>(body, head);
}
