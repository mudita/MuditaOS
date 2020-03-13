/*
 * @file PhonebookListView.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 11 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "PhonebookListView.hpp"
#include "../data/PhonebookItemData.hpp"
#include "../windows/PhonebookContact.hpp"
#include "PhonebookItem.hpp"
#include "UiCommonActions.hpp"
#include "application-call/ApplicationCall.hpp"
#include "log/log.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"

namespace gui
{

PhonebookListView::PhonebookListView() : ListView()
{
    orientation = ORIENTATION_TOP_DOWN;
}

PhonebookListView::PhonebookListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : ListView(parent, x, y, w, h)
{
    orientation = ORIENTATION_TOP_DOWN;
}

PhonebookListView::~PhonebookListView()
{
}

void PhonebookListView::setApplication(app::Application *app)
{
    application = app;
}

gui::PhonebookItem *PhonebookListView::getSelectedPhonebookItem()
{
    int index = selectedIndex - firstIndex;
    auto it = items.begin();
    if (items.size() == 0)
        return nullptr;

    int count = -1;
    for (int i = 0; i < pageSize; i++)
    {
        if ((*it)->getID() >= 0)
            count++;
        if (count == index)
            break;
        std::advance(it, 1);
    }

    return (dynamic_cast<PhonebookItem *>(*it));
}

bool PhonebookListView::onInput(const InputEvent &inputEvent)
{
    // LOG_INFO("code:%d state:%d", inputEvent.keyCode, inputEvent.state);
    if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_ENTER))
    {
        gui::PhonebookItem *item = getSelectedPhonebookItem();
        if (item)
        {
            if (cb_ENTER && cb_ENTER(item))
            {
                return true;
            }
            // LOG_INFO("show info on index: %d %s", item->getID(), item->getValue().c_str());

            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(item->getContact());
            application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));

            return true;
        }
        else
        {
            LOG_ERROR("failed to get selected item!");
            return false;
        }
    }

    if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_LF))
    {

        gui::PhonebookItem *item = getSelectedPhonebookItem();
        if (item)
        {
            // LOG_INFO("calling index: %d %s", item->getID(), item->getValue().c_str());
            return app::call(application, app::CallOperation::ExecuteCall, item->getContact()->numbers[0].numberE164);
        }
        else
        {
            LOG_ERROR("failed to get selected item!");
            return false;
        }
    }

    if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_UP))
    {
        if (orientation == ORIENTATION_TOP_DOWN)
        {
            if (listMode == MODE_PAGE)
            {
                if (selectedIndex > firstIndex)
                {
                    selectedIndex--;
                    return true;
                }
                if (selectedIndex == firstIndex)
                {
                    // ignore when someone wants to go above first element
                    if (selectedIndex == 0)
                        return false;
                    orientation = ORIENTATION_BOTTOM_UP;
                    if (firstIndex > 0)
                    {
                        selectedIndex = firstIndex - 1;
                        lastIndex = selectedIndex;
                        updateScrollDimenstions();
                        return true;
                    }
                }
            }
            else
            {
                // TODO implement continuous mode
            }
        }
        else if (orientation == ORIENTATION_BOTTOM_UP)
        {
            if (listMode == MODE_PAGE)
            {
                if (selectedIndex > firstIndex)
                {
                    selectedIndex--;
                    return true;
                }
                if (selectedIndex == firstIndex)
                {
                    if (firstIndex > 0)
                    {
                        selectedIndex = firstIndex - 1;
                        lastIndex = selectedIndex;
                        updateScrollDimenstions();
                        return true;
                    }
                }
            }
            else
            {
                // TODO implement continuous mode
            }
        }
    }
    else if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_DOWN))
    {
        if (orientation == ORIENTATION_TOP_DOWN)
        {
            if (listMode == MODE_PAGE)
            {
                if (selectedIndex < lastIndex)
                {
                    selectedIndex++;
                    return true;
                }
                if (selectedIndex == lastIndex)
                {
                    if (lastIndex < elementsCount - 1)
                    {
                        firstIndex = lastIndex + 1;
                        selectedIndex = firstIndex;
                        updateScrollDimenstions();
                        return true;
                    }
                }
            }
            else
            { // continuous mode
            }
        }
        else if (orientation == ORIENTATION_BOTTOM_UP)
        {
            if (listMode == MODE_PAGE)
            {
                if (selectedIndex < lastIndex)
                {
                    selectedIndex++;
                    return true;
                }
                if (selectedIndex == lastIndex)
                {
                    orientation = ORIENTATION_TOP_DOWN;
                    if (lastIndex < elementsCount - 1)
                    {
                        firstIndex = lastIndex + 1;
                        selectedIndex = firstIndex;
                        updateScrollDimenstions();
                        return true;
                    }
                }
            }
            else
            { // continuous mode
            }
        }
    }
    return false;
}

void PhonebookListView::updatePageItems()
{

    // defines how many slots in the list has been occupied
    int visibleElements = 0;

    // for top down orientation
    if (orientation == ORIENTATION_TOP_DOWN)
    {
        // index of requested item is incremented only if payload of returned item is not null
        // if it's null it means that this is favourite label of label connected with showing Capital letters for surnames
        int index = firstIndex, prevIndex = -1;
        lastIndex = firstIndex;
        for (int i = 0; i < pageSize; i++)
        {
            ListItem *item = provider->getItem(index, firstIndex, prevIndex, pageSize);

            if (item != nullptr)
            {
                addWidget(item);
                items.push_back(item);

                prevIndex = index;
                if (item->getID() >= 0)
                {
                    index++;
                    this->lastIndex++;
                }
                visibleElements++;
            }
        }
        // after the loop lastIndex is the number of elements not the index of last element that's why -- is used.
        lastIndex--;

        // if last element has been displayed but there is still space for elements
        if ((lastIndex == elementsCount - 1) && (visibleElements != pageSize) && (firstIndex != 0))
        {

            orientation = ORIENTATION_BOTTOM_UP;

            // clear all elements in the list.
            auto it = items.begin();
            for (uint32_t i = 0; i < items.size(); i++)
            {
                ListItem *item = *it;
                removeWidget(item);
                delete item;
                std::advance(it, 1);
            }
            items.clear();

            lastIndex = elementsCount - 1;
            firstIndex = lastIndex;
            index = lastIndex;
            prevIndex = -1;
            for (int i = 0; i < pageSize; i++)
            {
                ListItem *item = provider->getItem(index, lastIndex, prevIndex, pageSize, pageSize - 1 - i, false);

                if (item != nullptr)
                {
                    addWidget(item);
                    items.push_back(item);

                    prevIndex = index;
                    if (item->getID() >= 0)
                    {
                        index--;
                        firstIndex--;
                    }
                }
            }
        }
    }
    else if (orientation == ORIENTATION_BOTTOM_UP)
    {
        // index of requested item is incremented only if payload of returned item is not null
        // if it's null it means that this is favourite label of label connected with showing Capital letters for surnames
        int index = lastIndex, prevIndex = -1;
        firstIndex = lastIndex;
        for (int i = 0; i < pageSize; i++)
        {
            ListItem *item = provider->getItem(index, lastIndex, prevIndex, pageSize, pageSize - 1 - i, false);

            if (item != nullptr)
            {
                addWidget(item);
                items.push_back(item);

                prevIndex = index;
                if (item->getID() >= 0)
                {
                    index--;
                    firstIndex--;
                }
                visibleElements++;
            }
        }

        firstIndex++;

        // if last element has been displayed but there is still space for elements
        if ((lastIndex != elementsCount - 1) && (visibleElements != pageSize) && (firstIndex == 0))
        {

            orientation = ORIENTATION_TOP_DOWN;

            // clear all elements in the list.
            auto it = items.begin();
            for (uint32_t i = 0; i < items.size(); i++)
            {
                ListItem *item = *it;
                removeWidget(item);
                delete item;
                std::advance(it, 1);
            }
            items.clear();

            int index = firstIndex, prevIndex = -1;
            this->lastIndex = firstIndex;
            for (int i = 0; i < pageSize; i++)
            {
                ListItem *item = provider->getItem(index, firstIndex, prevIndex, pageSize);

                if (item != nullptr)
                {
                    addWidget(item);
                    items.push_back(item);

                    prevIndex = index;
                    if (item->getID() >= 0)
                    {
                        index++;
                        this->lastIndex++;
                    }
                }
            }
        }
    }

    if (firstIndex < 0)
        firstIndex = 0;

    // calculate height of the item using list's height and pageSize
    uint32_t itemWidth = widgetArea.w;
    if (drawVerticalScroll)
        itemWidth -= 10;

    // substract pageSize for 1 pixel separators
    int availableHeight = widgetArea.h - pageSize;
    if (availableHeight < 0)
        availableHeight = 0;
    int itemHeight = availableHeight / pageSize;

    int verticalPosition = 0;

    if (orientation == ORIENTATION_BOTTOM_UP)
        items.reverse();

    auto it = items.begin();

    for (unsigned int i = 0; i < items.size(); i++)
    {
        if (availableHeight > 0)
        {

            itemHeight = (*it)->minHeight;

            (*it)->setPosition(0, verticalPosition);
            verticalPosition += itemHeight + 1; // 1 for separator

            (*it)->setSize(itemWidth, itemHeight);

            // if list has focus and it is visible mark selected element
            if (visible)
            {
                if ((*it)->getID() == selectedIndex)
                    (*it)->setFocus(true);
            }

            availableHeight -= itemHeight;
        }
        std::advance(it, 1);
    }
}

} /* namespace gui */
