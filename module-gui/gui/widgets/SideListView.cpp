// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Image.hpp"
#include "SideListView.hpp"
#include "Style.hpp"

namespace gui
{
    SideListView::SideListView(Item *parent,
                               unsigned int x,
                               unsigned int y,
                               unsigned int w,
                               unsigned int h,
                               std::shared_ptr<SideListItemProvider> prov)
        : Rect{parent, x, y, w, h}
    {

        setEdges(RectangleEdge::None);

        auto body = new VBox{this, x, y, w, h};
        body->setEdges(RectangleEdge::None);

        progressbar = new HBarGraph(body, 0U, 0U, 0U);
        progressbar->setMargins(Margins(style::sidelistview::progress_bar::margin_left,
                                        style::sidelistview::progress_bar::margin_top,
                                        style::sidelistview::progress_bar::margin_right,
                                        style::sidelistview::progress_bar::margin_bottom));
        progressbar->activeItem = false;
        progressbar->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        listItemBox = new gui::HBox(body, 0U, 0U, style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        listItemBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        listItemBox->setEdges(gui::RectangleEdge::None);

        auto arrowsHBox = new HBox{this,
                                   style::sidelistview::arrows_hbox::x,
                                   style::sidelistview::arrows_hbox::y,
                                   style::sidelistview::arrows_hbox::w,
                                   style::sidelistview::arrows_hbox::h};
        arrowsHBox->setEdges(gui::RectangleEdge::None);
        auto arrowLeft = new gui::Image(style::sidelistview::arrow_left_image);
        arrowLeft->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        arrowLeft->activeItem = false;
        arrowLeft->setEdges(RectangleEdge::None);
        arrowsHBox->addWidget(arrowLeft);

        auto arrowRight = new gui::Image(style::sidelistview::arrow_right_image);
        arrowRight->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        arrowRight->activeItem = false;
        arrowRight->setEdges(RectangleEdge::None);

        auto rectFiller = new gui::Rect(arrowsHBox,
                                        0U,
                                        0U,
                                        arrowsHBox->getWidth() - arrowLeft->getWidth() - arrowRight->getWidth(),
                                        arrowsHBox->getHeight());

        rectFiller->setMaximumSize(arrowsHBox->getWidth(), arrowsHBox->getHeight());
        rectFiller->setEdges(RectangleEdge::None);

        arrowsHBox->addWidget(arrowRight);

        if (prov != nullptr) {
            provider       = std::move(prov);
            provider->list = this;
        }

        type = gui::ItemType::LIST;

        progressbar->setMaximum(provider->requestRecordsCount());
        progressbar->createGraph();
        progressbar->setValue(progressbarStartValue);

        if (auto firstItem = provider->getItem(Order::Next)) {
            listItemBox->addWidget(firstItem);
            listItemBox->setFocusItem(firstItem);
        }
    }

    bool SideListView::onInput(const gui::InputEvent &inputEvent)
    {
        if (auto ret = listItemBox->onInput(inputEvent)) {
            return ret;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            if (slide(Order::Next)) {
                progressbar->setValue(progressbar->getValue() + 1);
                return true;
            }
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            if (slide(Order::Previous)) {
                progressbar->setValue(progressbar->getValue() - 1);
                return true;
            }
        }

        return false;
    }

    auto SideListView::slide(gui::Order order) -> bool
    {
        auto newItem = provider->getItem(order);

        clear();
        if (newItem != nullptr) {
            listItemBox->addWidget(newItem);
            listItemBox->setFocusItem(newItem);
            return true;
        }
        return false;
    }

    SideListView::~SideListView()
    {
        clear();
    }

    void SideListView::clear()
    {
        listItemBox->setFocusItem(nullptr);

        while (auto el = listItemBox->children.back()) {
            if (el->type == ItemType::LIST_ITEM) {
                if (!dynamic_cast<ListItem *>(el)->deleteByList) {
                    listItemBox->removeWidget(el);
                }
                else {
                    listItemBox->erase(el);
                }
            }
            else {
                listItemBox->erase(el);
            }
        }
    }

} /* namespace gui */
