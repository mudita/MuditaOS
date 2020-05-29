#pragma once

#include "../data/PhonebookItemData.hpp"
#include "../models/PhonebookModel.hpp"
#include "../widgets/PhonebookListView.hpp"
#include "Text.hpp"

namespace gui
{
    namespace window
    {
        namespace name
        {
            inline std::string search = "Search";
        }
    } // namespace window

    class PhonebookSearch : public AppWindow
    {
      protected:
        /** labels */
        Text *inputField    = nullptr;

        PhonebookModel *phonebookModel = nullptr;

        /** currently displayed contact */
        std::shared_ptr<ContactRecord> contact = nullptr;

      public:
        PhonebookSearch(app::Application *app);
        ~PhonebookSearch() override = default;

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

    };

} /* namespace gui */
