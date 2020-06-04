#pragma once

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "gui/widgets/ListView.hpp"
#include "../models/PhonebookModel.hpp"
#include "../widgets/PhonebookListView.hpp"

namespace gui
{

    class PhonebookMainWindow : public AppWindow
    {
      protected:
        PhonebookModel *phonebookModel = nullptr;
        ListView *contactsList         = nullptr;

        Image *leftArrowImage  = nullptr;
        Image *rightArrowImage = nullptr;
        Image *newContactImage = nullptr;
        Image *searchImage     = nullptr;

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
    };

} /* namespace gui */
