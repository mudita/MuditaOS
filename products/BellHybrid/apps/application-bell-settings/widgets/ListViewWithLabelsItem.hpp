// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <common/widgets/ListViewWithLabels.hpp>

namespace gui
{
    class ListViewWithLabelsItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        ListViewWithLabelsItem(const std::string &description, std::shared_ptr<ListItemProvider> prov);

        std::string value() const
        {
            return ""; // TODO fix this, added so that compilation succeeds
        }

        //        void set_value(const value_type &value)
        //        {
        //            spinner->set_value(value);
        //            control_visibility();
        //        }
        //
        //        void set_range(const typename spinner_type::range &range)
        //        {
        //            spinner->set_range(range);
        //        }
        //
        //        void set_on_value_change_cb(std::function<void(const value_type &)> &&cb)
        //        {
        //            onValueChangeCb = std::move(cb);
        //        }

      private:
        ListViewWithLabels *list;
    };
} // namespace gui
