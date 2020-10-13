#pragma once

#include <string>

namespace gui
{

    class MeditationListRecord
    {
      public:
        enum class Type
        {
            Preparation,
            Option1,
            Option2
        };
        MeditationListRecord() = delete;
        explicit MeditationListRecord(Type type, std::string value) : id(nextId++), type(type), text(std::move(value))
        {}
        explicit MeditationListRecord(const MeditationListRecord &other)
            : id(other.id), type(other.type), text(other.text), value(other.value)
        {}
        [[nodiscard]] uint32_t getId() const noexcept
        {
            return id;
        }
        [[nodiscard]] Type getType() const noexcept
        {
            return type;
        }
        [[nodiscard]] std::string getTextValue() const
        {
            return text;
        }

        [[nodiscard]] int getValue() const noexcept
        {
            return value;
        }

        virtual ~MeditationListRecord() = default;

      private:
        static uint32_t nextId;
        uint32_t const id = 0;
        Type type;
        std::string text;

      protected:
        int value = 0;
    };

    class PreparationTimeRecord : public MeditationListRecord
    {
        static std::string convertToPrintable(int _durationInSeconds)
        {
            int minutes = _durationInSeconds / 60;
            if (minutes > 0) {
                return std::to_string(minutes) + " min";
            }
            return std::to_string(_durationInSeconds) + " s";
        }

      public:
        explicit PreparationTimeRecord(int _durationInSeconds)
            : MeditationListRecord(Type::Preparation, convertToPrintable(_durationInSeconds))
        {
            value = _durationInSeconds;
        }
    };

    class MeditationOptionRecord : public MeditationListRecord
    {
        static std::string convertToPrintable(Type _type)
        {
            if (_type == Type::Option1) {
                return "Show meditation counter";
            }
            else if (_type == Type::Option2) {
                return "Preparation Time";
            }
            else {
                return "Invalid Option";
            }
        }

      public:
        explicit MeditationOptionRecord(Type _type) : MeditationListRecord(_type, convertToPrintable(_type))
        {}
    };

} // namespace gui
