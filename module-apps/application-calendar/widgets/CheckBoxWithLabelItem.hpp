#pragma once
#include "CalendarListItem.hpp"
#include "Application.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <BoxLayout.hpp>
#include <BottomBar.hpp>
#include <CheckBox.hpp>

namespace gui
{
    class CheckBoxWithLabelItem : public CalendarListItem
    {
        gui::HBox *hBox           = nullptr;
        app::Application *app     = nullptr;
        bool checkIsOnLeftBarSide = false;

        virtual void applyCallbacks();

      public:
        CheckBoxWithLabelItem(app::Application *application,
                              const std::string &description,
                              bool checkIsOnLeftBarSide = false);
        virtual ~CheckBoxWithLabelItem() override = default;

        gui::Label *descriptionLabel = nullptr;
        gui::CheckBox *checkBox      = nullptr;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
