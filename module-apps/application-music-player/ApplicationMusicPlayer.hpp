#pragma once

#include <Application.hpp>

namespace gui
{
    namespace name
    {
        namespace window
        {}; // namespace window
    };      // namespace name
};          // namespace gui

namespace app
{
    const inline std::string name_music_player = "ApplicationMusicPlayer";

    class ApplicationMusicPlayer : public Application
    {
      protected:
      public:
        ApplicationMusicPlayer(std::string name    = name_music_player,
                               std::string parent  = "",
                               bool startBackgound = false);
        virtual ~ApplicationMusicPlayer();

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() final;
        void destroyUserInterface() final;

        std::vector<std::string> getMusicFilesList() const;
    };

} /* namespace app */
