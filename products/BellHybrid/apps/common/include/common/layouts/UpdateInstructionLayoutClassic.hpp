// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "UpdateInstructionLayoutProvider.hpp"

#include <BoxLayout.hpp>
#include <ImageBox.hpp>

namespace gui
{
    class BellBaseLayout;

    using Instruction = std::vector<std::pair<std::string, UTF8>>;

    class UpdateInstructionLayoutClassic : public UpdateInstructionLayoutProvider, VBox
    {
      public:
        UpdateInstructionLayoutClassic(const UTF8 &image,
                                       const UTF8 &title,
                                       const Instruction &instruction,
                                       bool leftArrowVisible  = true,
                                       bool rightArrowVisible = true);

        auto getLayout() -> Item * override;

      private:
        struct InstructionPoint
        {
            HBox *container;
            Text *textBox;
            Text *numberBox;
        };
        std::vector<InstructionPoint> instructionPoints;

        ImageBox *leftArrowBox;
        ImageBox *rightArrowBox;

        Text *titleBox;
        ImageBox *imageBox;

        void buildInterface(const Instruction &instruction);
    };
}; // namespace gui
