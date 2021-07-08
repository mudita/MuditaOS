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

        auto middleHbox = new gui::HBox(body,
                                        style::sidelistview::middleHBox::x,
                                        style::sidelistview::middleHBox::y,
                                        style::sidelistview::middleHBox::w,
                                        style::sidelistview::middleHBox::h);
        middleHbox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        middleHbox->setEdges(gui::RectangleEdge::None);

        auto arrowLeft = new gui::Image(style::sidelistview::arrowLeftImage);
        arrowLeft->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        arrowLeft->activeItem = false;
        middleHbox->addWidget(arrowLeft);

        listItemBox = new gui::Rect(middleHbox,
                                    style::sidelistview::listItem::x,
                                    style::sidelistview::listItem::y,
                                    style::sidelistview::listItem::w,
                                    style::sidelistview::listItem::h);
        listItemBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        listItemBox->setEdges(gui::RectangleEdge::None);

        auto arrowRight = new gui::Image(style::sidelistview::arrowRightImage);
        arrowRight->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        arrowRight->activeItem = false;
        middleHbox->addWidget(arrowRight);

        if (prov != nullptr) {
            provider       = std::move(prov);
            provider->list = this;
        }

        type = gui::ItemType::LIST;

        progressbar->setMaximum(provider->requestRecordsCount());
        progressbar->createGraph();
        progressbar->setValue(provider->getCurrentIndex() + 1);

        if (auto firstItem = provider->getItem(Order::Next)) {
            listItemBox->addWidget(firstItem);
            topMessage->setText(firstItem->getDescription());
            setFocusItem(firstItem);
        }
    }

    bool SideListView::onInput(const gui::InputEvent &inputEvent)
    {
        if (auto item = listItemBox->children.back()) {
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
        auto oldItem = listItemBox->children.back();
        auto newItem = provider->getItem(order);

        if (oldItem) {
            listItemBox->removeWidget(oldItem);
        }
        if (newItem != nullptr) {
            listItemBox->addWidget(newItem);
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
        listItemBox->setFocusItem(nullptr);

        while (auto el = listItemBox->children.back()) {
            if (el->type == ItemType::LIST_ITEM) {
                listItemBox->removeWidget(el);
            }
            else {
                listItemBox->erase(el);
            }
        }
    }

} /* namespace gui */
