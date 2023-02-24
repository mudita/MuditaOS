// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

namespace gui
{
    class MeditationOptionsWindow : public OptionWindow
    {
      public:
        explicit MeditationOptionsWindow(app::ApplicationCommon *);

      private:
        std::list<Option> buildOptionsList();
        void addCounterOption(std::list<Option> &options);
        void addPreparationTimeOption(std::list<Option> &options);
    };

    class PreparationTimeWindow : public OptionWindow
    {
      public:
        explicit PreparationTimeWindow(app::ApplicationCommon *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        std::list<Option> buildOptionsList();
        void addPreparationTimeOption(std::chrono::seconds duration,
                                      app::ApplicationMeditation *app,
                                      std::list<Option> &options);

        [[nodiscard]] std::size_t getSelectedItemIndex() const;

        std::size_t selectedItemIndex{0};
    };
} // namespace gui
