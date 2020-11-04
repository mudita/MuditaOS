// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <BoxLayout.hpp>
#include <memory>

namespace gui
{

    namespace window
    {
        inline constexpr auto hw_info = "hw_info";
    };

    class Info : public AppWindow
    {
      protected:
        VBox *box = nullptr;

      public:
        Info(app::Application *app);

        // virtual methods
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void addAlignedLabelWithValue(BoxLayout *layout, const std::string &labelText, const std::string &valueText);

        void invalidate() noexcept;
    };
} // namespace gui
