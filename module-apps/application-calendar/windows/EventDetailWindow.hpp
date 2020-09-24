#pragma once
#include "application-calendar/models/EventDetailModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <Style.hpp>
#include <ListView.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    class EventDetailWindow : public gui::AppWindow
    {
        std::string prevWindowName                         = "";
        std::shared_ptr<EventsRecord> eventRecord          = nullptr;
        std::shared_ptr<EventDetailModel> eventDetailModel = nullptr;
        gui::ListView *bodyList                            = nullptr;

      public:
        EventDetailWindow(app::Application *app, std::string name);
        virtual ~EventDetailWindow() override = default;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        auto handleSwitchData(SwitchData *data) -> bool override;
    };

} /* namespace gui */
