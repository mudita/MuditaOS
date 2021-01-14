// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/service-appmgr/messages/GetCurrentDisplayLanguageRequest.hpp>
#include <service-appmgr/service-appmgr/model/ApplicationManager.hpp>
#include "LanguagesModel.hpp"

void LanguagesModel::requestCurrentDisplayLanguage()
{
    application->bus.sendUnicast(std::make_shared<app::manager::GetCurrentDisplayLanguageRequest>(),
                                 app::manager::ApplicationManager::ServiceName);
}
