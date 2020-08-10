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
    class CheckBoxItem : public CalendarListItem
    {
        gui::HBox *hBox              = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::CheckBox *checkBox      = nullptr;
        app::Application *app        = nullptr;
        bool checkIsOnLeftBarSide    = false;

        void applyCallbacks();

      public:
        CheckBoxItem(app::Application *application, const std::string &description, bool checkIsOnLeftBarSide = false);
        virtual ~CheckBoxItem() override = default;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
