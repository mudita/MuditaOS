// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class CertificationWindow : public AppWindow
    {
      public:
        explicit CertificationWindow(app::ApplicationCommon *app);

      private:
        void buildInterface() override;

        void createTitle(Item *parent, const std::string &title);
        void createTitleAndImage(Item *parent, const std::string &title, const std::string &imageName);
        void createEntryWithTextsAndImage(Item *parent,
                                          const std::string &title,
                                          const std::string &description,
                                          const std::string &imageName);
        void createEntryWithTitlesAndImages(Item *parent,
                                            const std::string &title1,
                                            const std::string &imageName1,
                                            const std::string &title2,
                                            const std::string &imageName2);
    };

} // namespace gui
