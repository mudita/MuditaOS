// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RichTextParser.hpp"

#include <core/TextBlock.hpp>
#include <core/TextDocument.hpp>

#include <Font.hpp>
#include <Utils.hpp>
#include <utility>
#include <log/log.hpp>

#include <pugixml.hpp>
#include <list>

#ifdef DEBUG_RTP
const std::string node_types[] = {"null", "document", "element", "pcdata ", "cdata", "comment", "pi", "declaration"};
#define log_parser(...) LOG_DEBUG(__VA_ARGS__)
#else
#define log_parser(...)
#endif

namespace text
{
    class Attribute
    {
        std::string name;

      protected:
        explicit Attribute(std::string name) : name(std::move(name)){};

      public:
        Attribute() = delete;
        /// return true on success, otherwise set fallback value and return false
        /// @note could run: preVisitHook -> visit -> postVisit hook
        virtual auto visit(gui::TextFormat &fmt, std::string value) -> bool = 0;
        [[nodiscard]] auto getName() const -> const std::string &
        {
            return name;
        }

        [[nodiscard]] auto is(const std::string &name) const
        {
            return getName() == name;
        }
    };

    class AttributeAlign : public Attribute
    {
        const std::string center = gui::text::center;
        const std::string left   = gui::text::left;
        const std::string right  = gui::text::right;

      public:
        AttributeAlign() : Attribute(gui::text::align)
        {}

        auto visit(gui::TextFormat &fmt, std::string value) -> bool final
        {
            log_parser("append: Attr %s", getName().c_str());
            using namespace gui;
            auto alignment = Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom);
            if (value == center) {
                alignment.horizontal = Alignment::Horizontal::Center;
                fmt.setAlignment(alignment);
                return true;
            }
            else if (value == right) {
                alignment.horizontal = Alignment::Horizontal::Right;
                fmt.setAlignment(alignment);
                return true;
            }
            else if (value == left) {
                alignment.horizontal = Alignment::Horizontal::Left;
                fmt.setAlignment(alignment);
                return true;
            }

            fmt.setAlignment(alignment);
            return false;
        }
    };

    class AttributeColor : public Attribute
    {

        const int max = 15;

      public:
        AttributeColor() : Attribute(gui::text::color)
        {}

        auto visit(gui::TextFormat &fmt, std::string value) -> bool final
        {
            log_parser("append: Attr %s", getName().c_str());
            using namespace gui;
            if (value.empty()) {
                fmt.setColor(ColorFullBlack);
                return false;
            }
            try {
                int val = std::stoi(value);
                if ((val == 0 && value.c_str()[0] != '0') || val > max) {
                    fmt.setColor(ColorFullBlack);
                    return false;
                }
                fmt.setColor(Color{static_cast<uint8_t>(val), 0});
                return true;
            }
            catch (const std::exception &exception) {
                LOG_ERROR("%s", exception.what());
                return false;
            }
            return false;
        }
    };

    class AttributeFont : public Attribute
    {
      public:
        AttributeFont() : Attribute(gui::text::font)
        {}

        auto visit(gui::TextFormat &fmt, std::string value) -> bool final
        {
            log_parser("append: Attr %s", getName().c_str());
            using namespace gui;
            // here it's tricky -> we need to get font copy from fmt -> change it -> set it
            auto font = gui::Font(fmt.getFont());
            font.setFont(value, font.getSize());
            fmt.setFont(font.raw());
            return true;
        }
    };

    class AttributeSize : public Attribute
    {
      public:
        AttributeSize() : Attribute(gui::text::size)
        {}

        auto visit(gui::TextFormat &fmt, std::string value) -> bool final
        {
            log_parser("append: Attr %s", getName().c_str());
            if (value.empty()) {
                return false;
            }
            try {
                int val = std::stoi(value);

                if (val == 0 && value.front() != '0') {
                    return false;
                }

                auto font = gui::Font(fmt.getFont());
                font.setSize(val);
                fmt.setFont(font.raw());
                return true;
            }
            catch (const std::exception &exception) {
                LOG_ERROR("%s", exception.what());
                return false;
            }
        }
    };

    class AttributeWeight : public Attribute
    {
      protected:
        const std::string regular = "regular";
        const std::string bold    = "bold";
        const std::string light   = "light";

      public:
        AttributeWeight() : Attribute(gui::text::weight)
        {}

        auto visit(gui::TextFormat &fmt, std::string value) -> bool final
        {
            log_parser("append: Attr %s", getName().c_str());
            using namespace gui;
            auto font = gui::Font(fmt.getFont());
            if (value == regular) {
                font.setWeight(Font::Weight::Regular);
            }
            else if (value == light) {
                font.setWeight(Font::Weight::Light);
            }
            else if (value == bold) {
                font.setWeight(Font::Weight::Bold);
            }
            else {
                font.setWeight(Font::Weight::Regular);
                fmt.setFont(font.raw());
                return false;
            }

            fmt.setFont(font.raw());
            return true;
        }
    };

    class NodeDecor
    {
        std::list<std::unique_ptr<Attribute>> attrs;
        NodeDecor()
        {
            attrs.emplace_back(std::make_unique<AttributeAlign>());
            attrs.emplace_back(std::make_unique<AttributeColor>());
            attrs.emplace_back(std::make_unique<AttributeFont>());
            attrs.emplace_back(std::make_unique<AttributeSize>());
            attrs.emplace_back(std::make_unique<AttributeWeight>());
        }

      public:
        // for each met style -> put it on stack to be used
        // too deep -> can be optimized
        auto stack_visit(gui::TextFormat &format, const std::string &name, const std::string &value) -> bool
        {
            for (auto &attr : attrs) {
                if (attr->is(name)) {
                    if (!attr->visit(format, value)) {
                        LOG_ERROR("Attribute parsing error, default set");
                    }
                    return true;
                }
            }
            LOG_ERROR("Attribute not found");
            return false;
        }

        static auto get() -> NodeDecor &
        {
            static NodeDecor *ptr = nullptr;
            if (ptr == nullptr) {
                ptr = new NodeDecor();
            }
            return *ptr;
        }
    };

    class ShortTextNodes
    {
        using SingleAttributedNode = std::map<std::string, std::pair<std::string, std::string>>;
        static const SingleAttributedNode nodes;

      public:
        enum class AttributeContent
        {
            Name,
            Value
        };

        [[nodiscard]] static auto is(const char *nodeName) -> bool
        {
            return static_cast<bool>(nodes.count(std::string(nodeName)));
        }

        [[nodiscard]] static auto get(const char *nodeName, AttributeContent content) -> std::optional<std::string>
        {
            try {
                auto attribute = nodes.at(nodeName);
                return std::optional<std::string>(content == AttributeContent::Name ? attribute.first
                                                                                    : attribute.second);
            }
            catch (const std::out_of_range &) {
                LOG_ERROR("ShortTextNode not found");
                return {};
            }
        }
    };

    const ShortTextNodes::SingleAttributedNode ShortTextNodes::nodes = {
        {gui::text::short_bold, {gui::text::weight, gui::text::bold}}};

    class CustomTokens
    {
      public:
        using TokenMap = std::map<std::string, std::variant<int, std::string>>;
        explicit CustomTokens(TokenMap &&_tokens) : tokens{std::move(_tokens)}
        {}

        [[nodiscard]] static auto isCustomTokenNode(const std::string &nodeName) -> bool
        {
            return nodeName == gui::text::node_token;
        }

        [[nodiscard]] auto get(const std::string &contentName) -> std::optional<std::string>
        {
            try {
                auto token = tokens.at(contentName);
                return std::visit(
                    [](auto &&arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, int>) {
                            return std::make_optional(utils::to_string(arg));
                        }
                        else if constexpr (std::is_same_v<T, std::string>) {
                            return std::make_optional(arg);
                        }
                        else {
                            return std::nullopt;
                        }
                    },
                    std::move(token));
            }
            catch (const std::out_of_range &) {
                LOG_ERROR("Tokens not found");
            }
            return std::nullopt;
        }

      private:
        TokenMap tokens;
    };

}; // namespace text
struct walker : pugi::xml_tree_walker
{
  protected:
    std::list<gui::TextBlock> blocks;
    std::list<gui::TextFormat> style_stack;
    text::CustomTokens tokens;

    bool add_empty_line = false;
    bool adding_tokens  = false;

  public:
    walker(gui::TextFormat entry_style, ::text::CustomTokens::TokenMap &&tokens) : tokens{std::move(tokens)}
    {
        style_stack.push_back(entry_style);
    }

    enum class Action
    {
        Enter, /// enter/visit node
        Exit   /// exit/leave  node
    };

    auto log_node(pugi::xml_node &node, Action dir)
    {
        log_parser(
            "%s: %s format: %s",
            dir == Action::Enter ? "enter" : "leave",
            [&]() {
                std::stringstream ss;
                ss << node_types[node.type()] << ": name='" << node.name() << "', value='" << node.value() << "'";
                return ss.str();
            }()
                .c_str(),
            style_stack.back().str().c_str());
    }

    auto is_newline_node(pugi::xml_node &node) const
    {
        return std::string(node.name()) == gui::text::node_br || std::string(node.name()) == gui::text::node_p;
    }

    auto is_short_text_node(pugi::xml_node &node) const
    {
        return text::ShortTextNodes::is(node.name());
    }

    auto is_custom_token_node(pugi::xml_node &node) const
    {
        return text::CustomTokens::isCustomTokenNode(node.name());
    }

    auto push_text_node(pugi::xml_node &node)
    {
        auto local_style = style_stack.back();
        for (auto &attribute : node.attributes()) {
            log_parser("attribute name: %s value: %s", attribute.name(), attribute.value());
            auto &decor = text::NodeDecor::get();
            decor.stack_visit(local_style, attribute.name(), attribute.value());
        }
        style_stack.push_back(local_style);
        log_parser("Attr loaded: %s", style_stack.back().str().c_str());
    }

    auto push_short_text_node(pugi::xml_node &node)
    {
        log_parser("shortened text node name: %s", node.name());
        auto &decor    = text::NodeDecor::get();
        auto attrName  = text::ShortTextNodes::get(node.name(), text::ShortTextNodes::AttributeContent::Name);
        auto attrValue = text::ShortTextNodes::get(node.name(), text::ShortTextNodes::AttributeContent::Value);
        if (attrName.has_value() && attrValue.has_value()) {
            auto local_style = style_stack.back();
            decor.stack_visit(local_style, attrName.value(), attrValue.value());
            style_stack.push_back(local_style);
            log_parser("Attr loaded: %s", style_stack.back().str().c_str());
        }
    }

    auto push_newline_node(pugi::xml_node &)
    {
        if (!blocks.empty()) {
            if (blocks.back().getEnd() != gui::TextBlock::End::Newline) {
                blocks.back().setEnd(gui::TextBlock::End::Newline);
                add_empty_line = false;
            }
            else {
                add_empty_line = true;
            }
        }
        else {
            add_empty_line = true;
        }
    }

    auto start_custom_token_node(pugi::xml_node &)
    {
        adding_tokens = true;
    }

    auto push_custom_token_data_node(pugi::xml_node &node)
    {
        auto value = tokens.get(node.value());
        if (value.has_value()) {
            blocks.emplace_back(value.value(), std::make_unique<gui::TextFormat>(style_stack.back()));
        }
    }

    auto push_data_node(pugi::xml_node &node)
    {
        blocks.emplace_back(node.value(), std::make_unique<gui::TextFormat>(style_stack.back()));
    }

    auto for_each(pugi::xml_node &node) -> bool final
    {
        log_node(node, Action::Enter);
        if (node.type() == pugi::xml_node_type::node_element) {
            if (std::string(node.name()) == gui::text::node_text) {
                push_text_node(node);
                return true;
            }
            if (is_short_text_node(node)) {
                push_short_text_node(node);
                return true;
            }
            if (is_newline_node(node)) {
                push_newline_node(node);
                return true;
            }
            if (is_custom_token_node(node)) {
                start_custom_token_node(node);
                return true;
            }
        }

        std::string to_show = node.value();
        if (node.type() == pugi::xml_node_type::node_pcdata && !to_show.empty()) {
            if (adding_tokens) {
                push_custom_token_data_node(node);
            }
            else {
                push_data_node(node);
            }
        }
        return true;
    }

    auto pop_text_node(pugi::xml_node &node)
    {
        style_stack.pop_back();
    }

    auto pop_newline_node(pugi::xml_node &node)
    {
        if (add_empty_line) {
            blocks.emplace_back(gui::TextBlock("", std::make_unique<gui::TextFormat>(style_stack.back())));
            add_empty_line = false;
        }

        if (!blocks.empty()) {
            blocks.back().setEnd(gui::TextBlock::End::Newline);
        }
    }

    auto end_custom_token_node(pugi::xml_node &node)
    {
        adding_tokens = false;
    }

    auto on_leave(pugi::xml_node &node) -> bool final
    {
        log_node(node, Action::Exit);

        if (node.type() == pugi::xml_node_type::node_element) {
            if (std::string(node.name()) == gui::text::node_text || is_short_text_node(node)) {
                pop_text_node(node);
                return true;
            }
            if (is_newline_node(node)) {
                pop_newline_node(node);
                return true;
            }
            if (is_custom_token_node(node)) {
                end_custom_token_node(node);
            }
        }
        return true;
    }

    auto end(pugi::xml_node &node) -> bool final
    {
        return true;
    }

    auto souvenirs() -> std::list<gui::TextBlock> &
    {
        return blocks;
    }
};

namespace gui::text
{
    auto RichTextParser::parse(const UTF8 &text, TextFormat *base_style, TokenMap &&tokenMap)
        -> std::unique_ptr<TextDocument>
    {
        log_parser("parsing: %s", text.c_str());
        if (text.empty()) {
            log_parser("no: %s", "text");
            return nullptr;
        }

        pugi::xml_document doc;
        walker walker(base_style == nullptr ? nullptr : *base_style, std::move(tokenMap));

        doc.load_string(text.c_str());
        doc.traverse(walker);

        return std::make_unique<TextDocument>(walker.souvenirs());
    }
}; // namespace gui::text
