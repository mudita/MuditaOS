#include "buildTextDocument.hpp"
#include "TextDocument.hpp"
#include "multi-line-string.hpp"
#include <module-gui/gui/widgets/TextParse.hpp>
#include "InitializedFontManager.hpp"

namespace mockup
{

    std::tuple<gui::TextDocument, gui::Font *> buildEmptyTestDocument()
    {
        using namespace gui;
        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont(0);
        auto document     = TextDocument({});
        return std::tuple{document, testfont};
    }

    std::tuple<gui::TextDocument, gui::Font *> buildTestDocument()
    {
        using namespace gui;

        const auto no_lines = 3;

        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont(0);
        std::string text  = mockup::multiLineString(no_lines);
        auto blocks       = textToTextBlocks(text, testfont);
        auto document     = TextDocument(blocks);
        return std::tuple{document, testfont};
    }

    auto buildOnelineTestDocument(std::string text) -> std::tuple<gui::TextDocument, gui::Font *>
    {
        using namespace gui;

        auto &fontmanager = fontManager();
        auto testfont     = fontmanager.getFont(0);
        auto blocks       = textToTextBlocks(text, testfont, gui::TextBlock::End::None);
        auto document     = TextDocument(blocks);
        return std::tuple{document, testfont};
    }

    auto buildMultilineTestDocument(std::list<std::string> source) -> std::tuple<gui::TextDocument, gui::Font *>
    {
        using namespace gui;
        auto &fontmanager = mockup::fontManager();
        auto testfont     = fontmanager.getFont(0);
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
        auto testfont     = fontmanager.getFont(0);
        return TextDocument({TextBlock("", testfont, TextBlock::End::Newline)});
    }
} // namespace mockup
