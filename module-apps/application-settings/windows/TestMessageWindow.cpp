/*
 * TestMessageWindow.cpp
 *
 *  Created on: 3 gru 2019
 *      Author: kuba
 */

#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "TestMessageWindow.hpp"
#include <Style.hpp>

#include "service-cellular/api/CellularServiceAPI.hpp"

namespace gui {

TestMessageWindow::TestMessageWindow(app::Application *app) :
		AppWindow(app, "Message") {
	setSize(480, 600);

	buildInterface();
}

void TestMessageWindow::rebuild() {


	destroyInterface();
	buildInterface();

}

void textSetInput(Text * text, bool numeric = true)
{
    /// tu ustawiasz że coś jest edytowalne
    text->setEditMode(gui::Text::EditMode::EDIT);
    text->setFont(style::window::font::bigbold);
    /// tu ustawiasz tak naprawdę przez tą listę InputMode::digit etc jakie kolejne tryby wprowadzania są wybierane
    if(numeric) {
        text->setInputMode(new InputMode({InputMode::digit}));
    } else {
        text->setInputMode(new InputMode({InputMode::ABC, InputMode::abc, InputMode::digit}));
    }
}

void TestMessageWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive(BottomBar::Side::CENTER, true);
	bottomBar->setActive(BottomBar::Side::RIGHT, true);
	bottomBar->setText(BottomBar::Side::CENTER,
			utils::localize.get("common_select"));
	bottomBar->setText(BottomBar::Side::RIGHT,
			utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::SIGNAL, true);
	topBar->setActive(TopBar::Elements::BATTERY, true);

	receivedLabel = new gui::Label(this,10, 50, 480-20, 50, "Received SMS");
	receivedLabel->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	receivedLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

	receiveNumber = new gui::Label(this,110, 190, 480-120, 50);
	receiveNumber->setFont(style::window::font::small);

	RecNumberLabel = new gui::Label(this,10, 190, 90, 50, "Number");
	RecNumberLabel->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	RecNumberLabel->setFont(style::window::font::small);
	RecNumberLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

	receiveMessage = new gui::Text(this, 110, 120, 480-120, 50);
    textSetInput(receiveMessage);
    receiveMessage->setFont(style::window::font::small);

	RecMessageLabel = new gui::Label(this, 10, 120, 90, 50, "Message");
	RecMessageLabel->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	RecMessageLabel->setFont(style::window::font::small);
	RecMessageLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);


	sendLabel = new gui::Label(this,10, 290, 480-20, 50, "New SMS");
	sendLabel->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	sendLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

	sendMessage = new gui::Text(this,110, 360, 480-120, 50);
    textSetInput(sendMessage,false);
	sendMessage->setText("Siema tu PurePhone :)");
	sendMessage->setFont(style::window::font::small);

	sendNumber = new gui::Text(this,110, 430, 480-120, 50);
    textSetInput(sendNumber);
    sendNumber->setFont(style::window::font::small);

    SendNumberLabel = new gui::Label(this, 10, 430, 90, 50, "Number");
	SendNumberLabel->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	SendNumberLabel->setFont(style::window::font::small);
	SendNumberLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

	SendMessageLabel = new gui::Label(this, 10, 360, 90, 50, "Message");
	SendMessageLabel->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	SendMessageLabel->setFont(style::window::font::small);
	SendMessageLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

	sendButton = new gui::Label(this,10, 500, 480-20, 50, "Send SMS");
	sendButton->setAlignement(gui::Alignment( gui::Alignment::ALIGN_VERTICAL_CENTER, gui::Alignment::ALIGN_HORIZONTAL_CENTER));
	sendButton->setPenWidth(1);
	sendButton->setPenFocusWidth(3);
	sendButton->activatedCallback = [=] (gui::Item& item) {
		LOG_INFO("Send SMS callback" );
		CellularServiceAPI::SendSMS(application, sendNumber->getText(), sendMessage->getText());
		return true; };

	sendButton->setNavigationItem(gui::NavigationDirection::UP, sendNumber);
	sendNumber->setNavigationItem(gui::NavigationDirection::UP, sendMessage);

	sendMessage->setNavigationItem(gui::NavigationDirection::DOWN, sendNumber);
	sendNumber->setNavigationItem(gui::NavigationDirection::DOWN, sendButton);



}
void TestMessageWindow::destroyInterface() {
	AppWindow::destroyInterface();

	this->focusItem = nullptr;

	children.clear();
}

TestMessageWindow::~TestMessageWindow() {
}

void TestMessageWindow::onBeforeShow(ShowMode mode, SwitchData *data) {
	setFocusItem( sendButton );
}

void TestMessageWindow::cellularMessageCallback(UTF8& number, UTF8& message)
{
	receiveNumber->setText(number);
	receiveMessage->setText(message);
	application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
}
} /* namespace gui */
