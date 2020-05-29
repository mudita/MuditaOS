#pragma once

#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>
#include "OptionWindow.hpp"

namespace gui
{
    namespace window
    {
        namespace name
        {
            inline std::string options_namecard = "Options Namecard";
        }
    } // namespace window

    class PhonebookOptionsNamecard : public OptionWindow
    {
      public:
        PhonebookOptionsNamecard(app::Application *app);
        virtual ~PhonebookOptionsNamecard();
        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;

      private:
        std::shared_ptr<ContactRecord> contact = nullptr;
        std::list<gui::Option> namecardOptionsList();
        void sendViaSms();
        void sendViaBluetooth();
        const std::string formatVCARD();
    };
}; // namespace gui
