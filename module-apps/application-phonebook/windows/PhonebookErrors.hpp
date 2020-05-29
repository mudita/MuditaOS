#pragma once

#include "../data/PhonebookItemData.hpp"
#include "../models/PhonebookModel.hpp"
#include "../widgets/PhonebookListView.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>

namespace gui
{
    namespace window
    {
        namespace name
        {
            inline std::string noResults = "NoResults";
            inline std::string contactBlocked = "ContactBlocked";
        }
    } // namespace window

    class NoResults : public AppWindow
    {
      protected:
        /** labels */
        Label *topSeparatorLabel = nullptr;
        Text *informationLabel   = nullptr;
        Image *searchCenter;
        std::string searchQuery;

      public:
        NoResults(app::Application *app);
        virtual ~NoResults();
        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void setContactData();
    };

    class ContactBlocked : public AppWindow
    {
      protected:
        /** labels */
        Label *topSeparatorLabel = nullptr;
        Text *informationLabel   = nullptr;
        Image *searchCenter;
        std::string searchQuery;

      public:
        ContactBlocked(app::Application *app);
        virtual ~ContactBlocked();
        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void setContactData();
    };
}; // namespace gui
