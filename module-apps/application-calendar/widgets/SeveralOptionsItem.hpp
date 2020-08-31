#pragma once
#include "Application.hpp"
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class SeveralOptionsItem : public CalendarListItem
    {
        app::Application *application = nullptr;
        gui::VBox *vBox               = nullptr;
        gui::HBox *hBox               = nullptr;
        gui::Label *optionLabel       = nullptr;
        gui::Label *descriptionLabel  = nullptr;
        gui::Image *leftArrow         = nullptr;
        gui::Image *rightArrow        = nullptr;
        std::vector<std::string> optionsNames;
        unsigned int actualVectorIndex = 0;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

      public:
        SeveralOptionsItem(app::Application *app,
                           const std::string &description,
                           std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                           std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~SeveralOptionsItem() override = default;

        void prepareOptionsNames();
        void applyCallbacks();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
