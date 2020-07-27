#pragma once
#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class CheckBoxItem : public ListItem
    {
        gui::HBox *hBox              = nullptr;
        gui::Label *inputBoxLabel    = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Image *tickImage        = nullptr;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;
        // bool visibleState = false;

      public:
        CheckBoxItem(const std::string &description,
                     std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                     std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~CheckBoxItem() = default;

        void changeFont();
        // virtual methods from Item
        bool onActivated(void *data) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
