// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class BellFinishedWindow : public WindowWithTimer
    {
      public:
        struct Data : public gui::SwitchData
        {
          public:
            struct Factory
            {
                static std::unique_ptr<Data> create(const UTF8 &icon,
                                                    const UTF8 &text,
                                                    const std::string &windowToReturn)
                {
                    return std::unique_ptr<Data>(new Data(icon, text, windowToReturn));
                }
            };

            UTF8 icon{};
            UTF8 text{};
            std::string windowToReturn;

          private:
            Data() = default;

            Data(const UTF8 &icon, const UTF8 &text, const std::string &windowToReturn)
                : icon{icon}, text{text}, windowToReturn{windowToReturn}
            {}
        };

        static constexpr auto name = "BellFinishedWindow";

        explicit BellFinishedWindow(app::ApplicationCommon *app);

      private:
        void buildInterface() override;

        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        Icon *icon{};
        std::string windowToReturn;
    };

} // namespace gui
