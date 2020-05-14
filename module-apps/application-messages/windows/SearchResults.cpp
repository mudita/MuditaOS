#include "SearchResults.hpp"
#include <i18/i18.hpp>

namespace gui
{
    SearchResults::SearchResults(app::Application *app) : AppWindow(app, name::window::search_results)
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::search));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        body = new gui::Item();
        body->setBoundingBox(bodySize());
        addWidget(body);

        provider = std::make_unique<model::SearchResultsModel>(application);

        list = new gui::ListView(body,
                                 body->area().x + style::window::list_offset_default,
                                 8,
                                 body->area().w - 2 * style::window::list_offset_default,
                                 body->area().h);
        list->setPenFocusWidth(0);
        list->setPenWidth(1);
        list->setProvider(provider.get());
        // setFocusItem(body);
        LOG_DEBUG("%d", provider->getItemCount());

        list->clear();
        list->setElementsCount(provider->getItemCount());
        setFocusItem(list);
    }
}; // namespace gui
