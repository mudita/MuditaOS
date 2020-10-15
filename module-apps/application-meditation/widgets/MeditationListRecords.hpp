#pragma once

#include <string>
#include <i18/i18.hpp>

namespace gui
{
    class MeditationListRecord
    {
      public:
        enum class Type
        {
            Preparation,
            OptionMeditationCounter,
            OptionPreparation
        };

        MeditationListRecord() = delete;
        MeditationListRecord(Type type, std::string text) : id(nextId++), type(type), text(std::move(text))
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
        [[nodiscard]] std::string getText() const
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
        [[nodiscard]] static std::string convertToPrintable(int _durationInSeconds)
        {
            if (_durationInSeconds > 60) {
                return std::to_string(_durationInSeconds / 60) + " m";
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
        [[nodiscard]] static std::string convertToPrintable(Type _type)
        {
            if (_type == Type::OptionMeditationCounter) {
                return utils::localize.get("app_meditation_option_show_counter");
            }
            return utils::localize.get("app_meditation_preparation_time");
        }

      public:
        explicit MeditationOptionRecord(Type _type) : MeditationListRecord(_type, convertToPrintable(_type))
        {}
    };

} // namespace gui
