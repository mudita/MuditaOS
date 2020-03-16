/*
 * @file LockPhoneData.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 2 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_DATA_LOCKPHONEDATA_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_DATA_LOCKPHONEDATA_HPP_

#include "gui/SwitchData.hpp"

namespace gui
{

    // class template that stores information that was sent along with switch message
    class LockPhoneData : public gui::SwitchData
    {
        std::string previousApplication;

      public:
        LockPhoneData() : SwitchData()
        {
            description = "LockPhoneData";
        };
        virtual ~LockPhoneData(){};

        void setPrevApplication(const std::string &prevApp)
        {
            previousApplication = prevApp;
        };
        const std::string &getPreviousApplication()
        {
            return previousApplication;
        };
    };

} // namespace gui

#endif /* MODULE_APPS_APPLICATION_DESKTOP_DATA_LOCKPHONEDATA_HPP_ */
