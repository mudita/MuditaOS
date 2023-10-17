// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-phonebook/models/MultipleNumbersModel.hpp>

#include <AppWindow.hpp>
#include <ListView.hpp>

namespace gui
{
    class PhonebookMultipleNumbersSelect : public AppWindow
    {
      public:
        PhonebookMultipleNumbersSelect(app::ApplicationCommon *app,
                                       std::shared_ptr<MultipleNumbersModel> &&numberModel);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        std::shared_ptr<MultipleNumbersModel> numberModel;
        ListView *numberList = nullptr;
    };

} /* namespace gui */
