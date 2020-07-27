#pragma once
#include <Label.hpp>
#include <Text.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class EventTimeItem : public ListItem
    {
        gui::HBox *hBox              = nullptr;
        gui::Label *colonLabel       = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Text *hourInput         = nullptr;
        gui::Text *minuteInput       = nullptr;
        gui::Text *mode12hInput      = nullptr;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

      public:
        EventTimeItem(const std::string &description,
                      std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                      std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~EventTimeItem() = default;

        // virtual methods from Item
        bool onActivated(void *data) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
