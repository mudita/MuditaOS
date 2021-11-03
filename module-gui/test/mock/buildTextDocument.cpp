// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "buildTextDocument.hpp"
#include "InitializedFontManager.hpp"
#include "multi-line-string.hpp"

#include <widgets/text/parsers/TextParse.hpp>

namespace mockup
{

    std::tuple<gui::TextDocument, gui::RawFont *> buildEmptyTestDocument()
    {
        using namespace gui;
        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont();
        auto document     = TextDocument({});
        return std::tuple{document, testfont};
    }

    std::tuple<gui::TextDocument, gui::RawFont *> buildTestDocument(gui::TextBlock::End lineEnd)
    {
        using namespace gui;

        const auto no_lines = 3;

        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont();
        std::string text  = mockup::multiLineString(no_lines);
        auto blocks       = textToTextBlocks(text, testfont, lineEnd);
        auto document     = TextDocument(blocks);
        return std::tuple{document, testfont};
    }

    auto buildOnelineTestDocument(std::string text) -> std::tuple<gui::TextDocument, gui::RawFont *>
    {
        using namespace gui;

        auto &fontmanager = fontManager();
        auto testfont     = fontmanager.getFont();
        auto blocks       = textToTextBlocks(text, testfont, gui::TextBlock::End::None);
        auto document     = TextDocument(blocks);
        return std::tuple{document, testfont};
    }

    auto buildMultilineTestDocument(std::list<std::string> source) -> std::tuple<gui::TextDocument, gui::RawFont *>
    {
        using namespace gui;
        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont();
        std::list<TextBlock> blocks;
        for (auto el : source) {
            blocks.emplace_back(TextBlock(el, testfont, TextBlock::End::Newline));
        }
        auto document = TextDocument(blocks);
        return std::tuple{document, testfont};
    }

    auto buildJustNewlineTestDocument() -> gui::TextDocument
    {
        using namespace gui;
        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont();
        return TextDocument({TextBlock("", testfont, TextBlock::End::Newline)});
    }
} // namespace mockup
