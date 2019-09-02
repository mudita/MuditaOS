#pragma once

#include "AppWindow.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui {

class DebugLabel : public Label {
  public:
    std::string name;
    DebugLabel() = default;
    DebugLabel(std::string name) : name(name) {}
    DebugLabel(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, const UTF8 &text = UTF8{})
        : Label(parent, x, y, w, h, text), name("") {}
    virtual ~DebugLabel() {}
    virtual bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim);
};

/// simple class used for management of Input and Labels in view and passing controll
class ContactInputBox : public VBox {
    int focus_cnt;

  public:
    // This could be item but: getNavigationItem does something entirelly different (imho should be nade getNavigation)
    class SimpleParent {
      public:
        virtual ~SimpleParent() = default;
        virtual Item *getNavigationItem() { return nullptr; };
        virtual UTF8 value() { return ""; };
        virtual UTF8 name() { return ""; };
    };

    class Input : public SimpleParent {
        gui::Label *iname, *input;

      public:
        Input() = default;
        Input(Input &&) = default;
        virtual ~Input() = default;
        Input(ContactInputBox *parent, const UTF8 &text, const UTF8 &val = "");
        virtual Item *getNavigationItem() override { return input; }
        virtual UTF8 name() override { return iname->getText(); }
        virtual UTF8 value() override { return input->getText(); }
    };

    class Option : public SimpleParent {
        HBox *hbox;
        std::vector<Item *> boxItems;
        Label *name_label, *input_label;
        static const uint32_t x1 = 40, x3 = 40;
        uint32_t x2;

      public:
        Option() : hbox(nullptr), name_label(nullptr), input_label(nullptr), x2(0) {}
        Option(Option &&) = default;
        Option(ContactInputBox *parent, const UTF8 &text, const UTF8 &val = "");
        virtual ~Option();
        virtual Item *getNavigationItem() override;
        virtual UTF8 name() override { return name_label->getText(); }
        virtual UTF8 value() override { return input_label->getText(); }
    };

    enum Navigation {
        First,
        Last,
    };

  private:
    std::vector<SimpleParent *> elements;

  public:
    ContactInputBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);
    virtual ~ContactInputBox();
    /// adds Input to ContactInputBox
    void addInput(const UTF8 &text, const UTF8 &val = "");
    /// adds Option to ContactInputBox
    void addOption(const UTF8 &text, const UTF8 &val = "");
    /// returns first, or last navigation item - depending on Navigation enum
    /// in case of none returns nullptr
    Item *getNavigationItem(Navigation what);
    /// return all navigation elements
    /// in case of none returns emptu verctor
    std::vector<Item *> getNavigationItems();
    /// returns all data stored in elements as map of:  [language keys] -> values
    /// in case of none - returns empty map
    std::map<std::string, UTF8> getData();
    /// common parent set callback used to inform parent about change in focus on children
    std::function<bool(Item &)> parentSetCallback();
};

}; // namespace gui
