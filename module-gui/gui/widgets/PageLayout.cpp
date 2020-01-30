#include "PageLayout.hpp"
#include "Style.hpp"
#include <log/log.hpp>

namespace gui
{
    PageLayout::PageLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h) : BoxLayout(parent, x, y, w, h)
    {
        setPenWidth(style::window::default_border_no_focus_w);
        setPenFocusWidth(style::window::default_border_no_focus_w);

        inputCallback = [this](Item &item, const InputEvent &inputEvent) -> bool {
            int position = 0;
            bool handled = false;
            // first try with children and find position in pages
            for (auto &el : children)
            {
                if (el->visible)
                {
                    handled = el->onInput(inputEvent);
                    if (handled)
                    {
                        return true;
                    }
                    break;
                }
                ++position;
            }
            if (inputEvent.state != InputEvent::State::keyReleasedShort)
            {
                return false;
            }
            if (!handled)
            { // children didn't handle -> handle next/prev page with position
                if (inputEvent.keyCode == gui::KeyCode::KEY_UP && position != 0)
                {
                    return switchPage(position - 1, true);
                }
                if (inputEvent.keyCode == gui::KeyCode::KEY_DOWN && position < children.size() - 1)
                {
                    return switchPage(position + 1);
                }
            }
            return false;
        };
    }

    PageLayout::PageLayout(Item *parent, const BoundingBox &box) : PageLayout(parent, box.x, box.y, box.w, box.h)
    {
    }

    VBox *PageLayout::addPage()
    {
        auto el = new VBox();
        el->setPosition(0, 0);                   // setting position IN BOX if relative
        el->setSize(widgetArea.w, widgetArea.h); // set area to parent area
        el->setPenWidth(0);
        el->setRadius(0);
        // if new element fits && To avoid cyclic addWidget -> call parent addWidget
        if (BoxLayout::addWidget(el))
        {
            return el;
        }
        else
        {
            delete el;
            return nullptr;
        }
    }

    bool PageLayout::addWidget(Item *item)
    {
        if (this->children.size() == 0)
        {
            // cant add this element to this paged view at all
            if (addPage() == nullptr)
            {
                return false;
            }
        }
        auto el = dynamic_cast<VBox *>(children.back());
        bool added = el ? el->addWidget(item) : false;
        if (!added)
        {
            // add next page and try again - first set last box to not visible to avoid it's rebuild
            if (!addPage())
            {
                return false;
            }
            // set new (next) page invisible
            children.back()->setVisible(false);
            /// to not recure on addWidget
            auto el = dynamic_cast<VBox *>(children.back());
            if (!el->addWidget(item))
            {
                LOG_ERROR("Child not added on 2nd try, check sizes of Page and children u try to add!");
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
        return false;
    }

    bool PageLayout::switchPage(unsigned int n, bool previous)
    {
        if (children.size() == 0 || n >= children.size())
        {
            LOG_ERROR("Cant switch to page %d, num of pages: %d", n, children.size());
            return false;
        }
        unsigned int i = 0;
        for (auto &el : children)
        {
            if (i == n)
            {
                auto ell = dynamic_cast<VBox *>(el);
                if (ell)
                {
                    ell->setVisible(true, previous);
                }
            }
            else
            {
                el->setVisible(false);
            }
            ++i;
        }
        return true;
    }
}; // namespace gui
