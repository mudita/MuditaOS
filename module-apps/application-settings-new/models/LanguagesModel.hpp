// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

class LanguagesModel
{
  public:
    explicit LanguagesModel(app::Application *application) : application(application)
    {}

    void requestCurrentDisplayLanguage();

  private:
    app::Application *application = nullptr;
};
