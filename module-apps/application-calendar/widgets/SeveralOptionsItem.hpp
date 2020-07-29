#pragma once
#include "Application.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>
#include <BottomBar.hpp>

namespace gui
{
    class SeveralOptionsItem : public ListItem
    {
        app::Application *application = nullptr;
        gui::HBox *hBox               = nullptr;
        gui::Label *optionLabel       = nullptr;
        gui::Label *descriptionLabel  = nullptr;
        gui::Image *leftArrow         = nullptr;
        gui::Image *rightArrow        = nullptr;
        std::vector<std::string> optionsNames;
        unsigned int actualVectorIndex = 0;

        std::function<void(const UTF8 &text, BottomBar::Side side, bool emptyOthers)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode                        = nullptr;

      public:
        SeveralOptionsItem(app::Application *app,
                           const std::string &description,
                           std::function<void(const UTF8 &text, BottomBar::Side side, bool emptyOthers)>
                               bottomBarTemporaryMode                              = nullptr,
                           std::function<void()> bottomBarRestoreFromTemporaryMode = nullptr);
        virtual ~SeveralOptionsItem() override = default;

        void prepareOptionsNames();
        void setNavigation();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
