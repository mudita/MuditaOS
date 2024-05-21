// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/UpdateInstructionLayouts.hpp"
#include "layouts/UpdateInstructionLayoutClassic.hpp"

namespace gui::factory
{
    std::vector<LayoutGenerator> getUpdateInstructionLayouts()
    {
        return {{[]() {
                    const Instruction instructions{{"1.", "app_bell_update_instruction_1st_point"},
                                                   {"2.", "app_bell_update_instruction_2nd_point"}};
                    return new UpdateInstructionLayoutClassic(
                        "bell_mudita_logo", "app_bell_update_instruction_title", instructions, false, true);
                }},
                {[]() {
                    const Instruction instructions{{"3.", "app_bell_update_instruction_3rd_point"},
                                                   {"4.", "app_bell_update_instruction_4th_point"}};
                    return new UpdateInstructionLayoutClassic(
                        "bell_mudita_logo", "app_bell_update_instruction_title", instructions, true, false);
                }}};
    }
} // namespace gui::factory
