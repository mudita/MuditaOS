#pragma once

#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

#include <gui/widgets/ListView.hpp>

namespace gui
{

    class PhonebookIceContacts : public AppWindow
    {
      public:
        PhonebookIceContacts(app::Application *app);
        virtual ~PhonebookIceContacts();

        bool onInput(const InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        std::shared_ptr<PhonebookModel> phonebookModel;
        ListView *contactsListIce = nullptr;
        Image *leftArrowImage     = nullptr;
    };

} /* namespace gui */
