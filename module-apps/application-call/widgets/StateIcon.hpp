// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <i18n/i18n.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <log/log.hpp>
#include <Style.hpp>
#include <ImageBoxWithText.hpp>

#include <map>

namespace gui
{
    namespace icon
    {
        constexpr uint32_t w                 = 150;
        constexpr uint32_t h                 = 100;
        constexpr uint32_t defaultSideMargin = 5;
    } // namespace icon

    /// @brief Icon widget with custom predefined images and strings
    /// images and strings are stored in predefined map passed to the class during construction
    /// class T is used as on object state as well as a key to the map.
    ///
    /// @note Desired visible area is icon::w x icon::h. However, the whole widget area might be set to bigger one w x
    /// icon::h where w = icon::w + 2 * w_margin. It is necessary as it is possible that text will exceed Icon visible
    /// area

    template <class T>
    class StateIcon : public ImageBoxWithText
    {
      public:
        using IconMap = std::map<T, std::pair<const std::string, const std::string>>;
        StateIcon()   = delete;
        StateIcon(Item *parent, T state, const IconMap &data)
            : ImageBoxWithText(parent,
                               new gui::Image(data.at(state).first, ImageTypeSpecifier::W_M),
                               utils::translate(data.at(state).second)),
              state(state), data(data)
        {
            setSize(icon::w, icon::h);
            setMargins({icon::defaultSideMargin, 0, icon::defaultSideMargin, 0});
            set(state);
        }

        virtual ~StateIcon()
        {}

        /// @brief sets Icon state and display appropriate image and string
        /// @param state to set. @note state is also used as a key to the internal map. If the state points to the
        /// different than 1st mapping, string is displayed as bold one.
        virtual void set(const T &state)
        {
            this->state = state;
            image->set(data.at(state).first, ImageTypeSpecifier::W_M);

            text->setFont(data.find(state) != data.begin() ? imageBoxWithText::fontBold : imageBoxWithText::font);
            text->setText(utils::translate(data.at(state).second));
            text->setMinimumWidthToFitText(utils::translate(data.at(state).second));
            text->setMinimumHeightToFitText();
            text->informContentChanged();
        }

        T get() const
        {
            return state;
        }

        /// @brief sets Icon state to the next one (from the map) and displays next predfined image and string
        void setNext()
        {
            auto it = data.find(state);
            it++;
            if (it != data.end()) {
                set(it->first);
            }
            else {
                set(data.begin()->first);
            }
        }

        // sets/clears focus of internal boundingrect
        bool onFocus(bool state) override
        {
            Item::onFocus(state);

            return true;
        }

      protected:
        T state;
        const IconMap data; // internal map with predefined sets of images and strings to be displayed
    };
} // namespace gui
