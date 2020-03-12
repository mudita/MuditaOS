/*
 * @file SwitchData.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 15 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_SWITCHDATA_HPP_
#define MODULE_APPS_SWITCHDATA_HPP_

#include <string>

namespace gui
{

    // class template that stores information that was sent along with switch message
    class SwitchData
    {
      protected:
        std::string description = "";

      public:
        SwitchData() = default;
        SwitchData(const std::string &description) : description{description} {};
        virtual ~SwitchData() = default;
        virtual const std::string &getDescription() const
        {
            return description;
        };
        virtual void setDescription(const std::string desc)
        {
            description = desc;
        };
        bool disableAppClose = false;
    };

    class SwitchSpecialChar : public SwitchData
    {
      public:
        std::string requester = "";
        enum class Type
        {
            Request,
            Response,
        } type = Type::Request;
        SwitchSpecialChar(Type type, const std::string requester, const std::string &description = "")
            : SwitchData(description), requester(requester), type(type)
        {}
        virtual ~SwitchSpecialChar() = default;
    };

} /* namespace gui */

#endif /* MODULE_APPS_SWITCHDATA_HPP_ */
