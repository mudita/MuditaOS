// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <string>
#include <list>

namespace gui::dom
{
    class DomNode
    {
      protected:
        int level;
        std::string tag;
        DomNode(int level, std::string tag) : level{level}, tag{std::move(tag)}
        {}

      public:
        DomNode() = delete;
        virtual void visit(std::list<std::string> &document) const
        {
            std::string line;
            for (int i = 0; i < level; i++) {
                line += "\t";
            }
            line += tag;
            document.emplace_back(std::move(line));
        }
        virtual ~DomNode() = default;
    };

    class DomChildNode : public DomNode
    {
      public:
        DomChildNode(int level, std::string tag) : DomNode(level, std::move(tag))
        {}
    };

    class DomAttributeNode : public DomNode
    {
        std::optional<std::string> value;

      public:
        DomAttributeNode(int level, std::string tag) : DomNode(level, std::move(tag))
        {}
        DomAttributeNode(int level, std::string tag, std::string value)
            : DomNode(level, std::move(tag)), value{std::make_optional(std::move(value))}
        {}
        DomAttributeNode(int level, std::string tag, int value)
            : DomNode(level, std::move(tag)), value{std::make_optional(std::to_string(value))}
        {}
        void visit(std::list<std::string> &document) const override
        {
            DomNode::visit(document);
            if (value.has_value()) {
                document.back() += " : " + value.value();
            }
        }
    };

    class DomFunctionalNode : public DomNode
    {
      public:
        enum class Type
        {
            NodeBegin,
            NodeEnd
        };
        DomFunctionalNode(int level, Type type) : DomNode(level, toString(type))
        {}

      private:
        static std::string toString(Type _type)
        {
            if (_type == Type::NodeBegin) {
                return std::string{"{"};
            }
            else {
                return std::string{"}"};
            }
        }
    };

} // namespace gui::dom
