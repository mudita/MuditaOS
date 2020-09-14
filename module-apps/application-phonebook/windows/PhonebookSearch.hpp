#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

#include <Text.hpp>

namespace gui
{
    class PhonebookSearch : public AppWindow
    {
      public:
        PhonebookSearch(app::Application *app);
        ~PhonebookSearch() override = default;

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        Text *inputField                       = nullptr;
        std::shared_ptr<ContactRecord> contact = nullptr;
    };

} /* namespace gui */
