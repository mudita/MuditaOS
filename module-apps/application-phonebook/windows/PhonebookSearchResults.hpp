#pragma once

#include "../data/PhonebookItemData.hpp"
#include "../models/SearchResultsModel.hpp"
#include "../widgets/PhonebookListView.hpp"

namespace gui
{
    namespace window
    {
        namespace name
        {
            const inline std::string search_results = "SearchResults";
        };
    }; // namespace name

class PhonebookSearchResults : public AppWindow
{
  protected:
    SearchResultsModel *searchResultsModel = nullptr;
    PhonebookListView *searchResultList    = nullptr;

    Image *newContactImage = nullptr;

  public:
    PhonebookSearchResults(app::Application *app);
    virtual ~PhonebookSearchResults();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
    };

} /* namespace gui */
