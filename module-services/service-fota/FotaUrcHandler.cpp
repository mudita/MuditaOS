// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FotaUrcHandler.hpp"

using namespace at::urc;

void FotaUrcHandler::Handle(Qind &urc)
{
    if (urc.isFotaValid()) {
        if (urc.getFotaStage() == Qind::FotaStage::START) {
            LOG_DEBUG("FOTA UPDATING");
        }
        else if (urc.getFotaStage() == Qind::FotaStage::HTTPEND) {
            LOG_DEBUG("Downloading finished: %s", urc.getFotaParameter().c_str());
        }
        else if (urc.getFotaStage() == Qind::FotaStage::END) {
            LOG_DEBUG("FOTA FINISHED -> reboot (%s)", fotaService.receiverServiceName.c_str());
            fotaService.sendFotaFinshed(fotaService.receiverServiceName);
        }
        else if (urc.getFotaStage() == Qind::FotaStage::UPDATING) {
            auto token_val = 0;
            try {
                token_val = std::stoi(urc.getFotaParameter());
            }
            catch (const std::exception &e) {
                LOG_ERROR("Conversion error of %s, taking default value %d", urc.getFotaParameter().c_str(), token_val);
            }

            unsigned char progress = static_cast<unsigned char>(token_val);
            LOG_DEBUG("FOTA UPDATING: %d", progress);
            fotaService.sendProgress(progress, fotaService.receiverServiceName);
        }
        else if (urc.getFotaStage() == Qind::FotaStage::HTTPSTART) {
            LOG_DEBUG("Start downloading DELTA");
        }
    }
}
