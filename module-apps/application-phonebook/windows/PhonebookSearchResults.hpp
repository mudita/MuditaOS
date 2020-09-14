#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

namespace gui
{
    class PhonebookSearchResults : public AppWindow
    {
      protected:
        std::shared_ptr<PhonebookModel> searchResultsModel;
        PhonebookListView *searchResultList = nullptr;

      public:
        PhonebookSearchResults(app::Application *app);
        ~PhonebookSearchResults() override;

        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
