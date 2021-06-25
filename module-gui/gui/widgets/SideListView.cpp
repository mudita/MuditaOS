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

        setBorderColor(ColorNoColor);

        auto body = new VBox{this, x, y, w, h};
        body->setEdges(RectangleEdge::None);

        progressbar = new HBarGraph(body, style::sidelistview::progressBar::x, style::sidelistview::progressBar::y, 0);
        progressbar->activeItem = false;
        progressbar->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        topMessage = new Label(body,
                               style::sidelistview::topMessage::x,
                               style::sidelistview::topMessage::y,
                               style::sidelistview::topMessage::w,
                               style::sidelistview::topMessage::h);
        topMessage->setFont(style::window::font::supersizemelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        auto middle_hbox = new gui::HBox(body,
                                         style::sidelistview::middleHBox::x,
                                         style::sidelistview::middleHBox::y,
                                         style::sidelistview::middleHBox::w,
                                         style::sidelistview::middleHBox::h);
        middle_hbox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        middle_hbox->setEdges(gui::RectangleEdge::None);

        auto arrow_left = new gui::Image(style::sidelistview::arrowLeftImage);
        arrow_left->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        arrow_left->activeItem = false;
        middle_hbox->addWidget(arrow_left);

        listitemBox = new gui::Rect(middle_hbox,
                                    style::sidelistview::listItem::x,
                                    style::sidelistview::listItem::y,
                                    style::sidelistview::listItem::w,
                                    style::sidelistview::listItem::h);
        listitemBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        listitemBox->setEdges(gui::RectangleEdge::None);

        auto arrow_right = new gui::Image(style::sidelistview::arrowRightImage);
        arrow_right->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        arrow_right->activeItem = false;
        middle_hbox->addWidget(arrow_right);

        if (prov != nullptr) {
            provider       = std::move(prov);
            provider->list = this;
        }

        type = gui::ItemType::LIST;

        progressbar->setMaximum(provider->requestRecordsCount());
        progressbar->createGraph();
        progressbar->setValue(provider->getCurrentIndex() + 1);

        if (auto firstItem = provider->getItem(Order::Next)) {
            listitemBox->addWidget(firstItem);
            topMessage->setText(firstItem->getDescription());
            setFocusItem(firstItem);
        }
    }

    bool SideListView::onInput(const gui::InputEvent &inputEvent)
    {
        if (auto item = listitemBox->children.back()) {
            if (auto ret = item->onInput(inputEvent)) {
                return ret;
            }
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            if (slide(Order::Next)) {
                progressbar->setValue(provider->getCurrentIndex() + 1);
                return true;
            }
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            if (slide(Order::Previous)) {
                progressbar->setValue(provider->getCurrentIndex() + 1);
                return true;
            }
        }

        return false;
    }

    auto SideListView::slide(gui::Order order) -> bool
    {
        auto oldItem = listitemBox->children.back();
        auto newItem = provider->getItem(order);

        if (oldItem) {
            listitemBox->removeWidget(oldItem);
        }
        if (newItem != nullptr) {
            listitemBox->addWidget(newItem);
            topMessage->setText(newItem->getDescription());
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
        listitemBox->setFocusItem(nullptr);

        while (auto el = listitemBox->children.back()) {
            if (el->type == ItemType::LIST_ITEM) {
                listitemBox->removeWidget(el);
            }
            else {
                listitemBox->erase(el);
            }
        }
    }

} /* namespace gui */
