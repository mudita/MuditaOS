// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogModel.hpp"
#include "data/CallLogInternals.hpp"
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>
#include <ListItem.hpp>

namespace gui
{

    namespace clItemStyle
    {
        inline constexpr auto w               = style::window::default_body_width;
        inline constexpr auto h               = style::window::label::big_h;
        inline constexpr auto internal_margin = 4;
        inline constexpr auto right_margin    = 10;

        namespace timestamp
        {
            inline constexpr auto min_w = 120;
        }
    } // namespace clItemStyle

    /*
     * @brief Widget used to display information about calllog entry in the calllog list view.
     */
    class CalllogItem : public ListItem
    {
        CalllogModel *model = nullptr;
        // pointer to the calls record
        std::shared_ptr<CalllogRecord> call = nullptr;
        gui::HBox *hBox                     = nullptr;
        // this is timestamp in the mode defined in settings
        gui::Label *timestamp                                                  = nullptr;
        gui::Image *imageCallType[calllog::CallLogCallType::NUM_OF_CALL_TYPES] = {nullptr, nullptr, nullptr};
        gui::Label *text                                                       = nullptr;

      public:
        CalllogItem(CalllogModel *model);
        virtual ~CalllogItem() = default;
        // sets copy of alarm's
        void setCall(std::shared_ptr<CalllogRecord> &);
        [[nodiscard]] auto getCall() const -> CalllogRecord
        {
            return call != nullptr ? *call : CalllogRecord();
        };
    };

} /* namespace gui */
