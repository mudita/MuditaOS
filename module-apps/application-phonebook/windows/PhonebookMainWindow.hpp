#pragma once

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

#include <gui/widgets/ListView.hpp>

namespace gui
{

    class PhonebookMainWindow : public AppWindow
    {
      protected:
        std::shared_ptr<PhonebookModel> phonebookModel = nullptr;
        ListView *contactsList                         = nullptr;

        Image *leftArrowImage  = nullptr;
        Image *rightArrowImage = nullptr;
        Image *newContactImage = nullptr;
        Image *searchImage     = nullptr;
        bool enableNewContact  = true;
        bool requestedSearch   = false;

      public:
        PhonebookMainWindow(app::Application *app);
        virtual ~PhonebookMainWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        bool isSearchRequested() const;
    };

} /* namespace gui */
