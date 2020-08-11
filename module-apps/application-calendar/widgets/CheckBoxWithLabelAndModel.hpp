#pragma once
#include "CheckBoxWithLabelItem.hpp"
#include "application-calendar/models/NewEditEventModel.hpp"

namespace gui
{
    class CheckBoxWithLabelAndModel : public CheckBoxWithLabelItem
    {
        NewEditEventModel *model = nullptr;
        app::Application *app    = nullptr;
        void applyCallbacks() override;

      public:
        CheckBoxWithLabelAndModel(app::Application *application,
                                  const std::string &description,
                                  bool checkIsOnLeftBarSide = false,
                                  NewEditEventModel *model  = nullptr);
        virtual ~CheckBoxWithLabelAndModel() override = default;
    };

} /* namespace gui */
