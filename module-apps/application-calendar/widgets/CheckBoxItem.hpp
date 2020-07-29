#pragma once
#include "Application.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>
#include <BottomBar.hpp>
#include <CheckBox.hpp>

namespace gui
{
    class CheckBoxItem : public ListItem
    {
        gui::HBox *hBox              = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::CheckBox *checkBox      = nullptr;
        app::Application *app        = nullptr;

      public:
        CheckBoxItem(app::Application *application, const std::string &description);
        virtual ~CheckBoxItem() override = default;

        void checkOnLeftSettings();
        void checkInCenterSettings();
        bool isChecked();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
