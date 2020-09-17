#pragma once
#include "CheckBoxWithLabelItem.hpp"
#include "application-calendar/models/NewEditEventModel.hpp"

namespace gui
{
    class NewEventCheckBoxWithLabel : public CheckBoxWithLabelItem
    {
        NewEditEventModel *model = nullptr;
        app::Application *app    = nullptr;
        void applyCallbacks() override;

      public:
        NewEventCheckBoxWithLabel(app::Application *application,
                                  const std::string &description,
                                  bool checkIsOnLeftBarSide = false,
                                  NewEditEventModel *model  = nullptr);
        virtual ~NewEventCheckBoxWithLabel() override = default;
    };

} /* namespace gui */
