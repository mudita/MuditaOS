#pragma once

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "gui/widgets/ListView.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

namespace gui
{

    class PhonebookIceContacts : public AppWindow
    {
      protected:
        std::shared_ptr<PhonebookModel> phonebookModel;
        ListView *contactsListIce = nullptr;

        Image *leftArrowImage  = nullptr;
        Image *rightArrowImage = nullptr;
        Image *newContactImage = nullptr;
        Image *searchImage     = nullptr;
        bool enableNewContact  = true;
        bool requestedSearch   = false;

      public:
        PhonebookIceContacts(app::Application *app);
        virtual ~PhonebookIceContacts();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        bool isSearchRequested() const;
    };

} /* namespace gui */
