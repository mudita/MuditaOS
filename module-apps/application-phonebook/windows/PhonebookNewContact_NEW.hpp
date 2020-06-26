#pragma once

#include "application-phonebook/models/NewContactModel.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"

#include <Text.hpp>

#include <gui/widgets/ListView.hpp>

namespace gui
{
    class PhonebookNewContact_NEW : public AppWindow
    {
      public:
      protected:
        gui::Label *title = nullptr;

        std::shared_ptr<ContactRecord> contact = nullptr;

      public:
        PhonebookNewContact_NEW(app::Application *app);
        virtual ~PhonebookNewContact_NEW();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        //        auto verifyAndSave() -> bool;
        //        const std::string getCountryPrefix();
        //        void showDialogDuplicatedNumber(ContactRecord &newContactRecord, const UTF8 duplicatedNumber);
        //        void showDialogDuplicatedSpeedDialNumber(ContactRecord &newContactRecord);

        gui::ListView *list              = nullptr;
        NewContactModel *newContactModel = nullptr;
    };

} /* namespace gui */
