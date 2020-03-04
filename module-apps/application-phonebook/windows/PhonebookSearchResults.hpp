#pragma once
#include <functional>
#include <string>

#include "../data/PhonebookItemData.hpp"
#include "../models/SearchResultsModel.hpp"
#include "../widgets/PhonebookListView.hpp"
#include "AppWindow.hpp"
#include "ListView.hpp"
#include "Style.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"

namespace gui
{
    namespace name
    {
        namespace window
        {
            const inline std::string search_results = "SearchResults";
        };
    }; // namespace name

class PhonebookSearchResults : public AppWindow
{
  protected:
    PhonebookListView *list;
    SearchResultsModel *searchResultsModel = nullptr;

    Image *leftArrowImage = nullptr;
    Image *rightArrowImage = nullptr;
    Image *newContactImage = nullptr;
    Image *searchImage = nullptr;

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
