#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace bt::tx_test
{
    enum class Pattern : char
    {
        PN9   = 0x0,
        PN15  = 0x1,
        Zebra = 0x2, // (10101010)
        All1  = 0x3,
        All0  = 0x4,
        FOFO  = 0x5,
        FF00  = 0x6,
    };

    /// freq starts on 2402
    enum class Chanel : char
    {
        f2402 = 0,  // first chanel
        f2442 = 20, // middle chanel
        f2480 = 78, // last chanel
    };

    enum class Power : char
    {
        Min = 1,
        Med = 7,
        Max = 15,
    };
}; // namespace bt::tx_test

inline const char *c_str(bt::tx_test::Pattern p)
{
    switch (p) {
    case bt::tx_test::Pattern::PN9:
        return "PN9";
    case bt::tx_test::Pattern::PN15:
        return "PN15";
    case bt::tx_test::Pattern::Zebra:
        return "Zebra";
    case bt::tx_test::Pattern::All1:
        return "All1";
    case bt::tx_test::Pattern::All0:
        return "All0";
    case bt::tx_test::Pattern::FOFO:
        return "FOFO";
    case bt::tx_test::Pattern::FF00:
        return "FF00";
    }
    return "";
}

inline const char *c_str(bt::tx_test::Chanel c)
{
    switch (c) {
    case bt::tx_test::Chanel::f2402:
        return "f2402";
    case bt::tx_test::Chanel::f2442:
        return "f2442";
    case bt::tx_test::Chanel::f2480:
        return "f2480";
    }
    return "";
}

inline const char *c_str(bt::tx_test::Power p)
{
    switch (p) {
    case bt::tx_test::Power::Min:
        return "Min";
    case bt::tx_test::Power::Med:
        return "Mid";
    case bt::tx_test::Power::Max:
        return "Max";
    }
    return "";
}

namespace message::bt
{
    class Test : public sys::DataMessage
    {
      public:
        /// power on bt can be steered between 0x0 - 0xF
        ::bt::tx_test::Power power;
        ::bt::tx_test::Chanel chanel;
        ::bt::tx_test::Pattern pattern;

        Test() 
            : sys::DataMessage(MessageType::BtMessage)
        {};

        Test(::bt::tx_test::Power power, ::bt::tx_test::Chanel chanel, ::bt::tx_test::Pattern pattern)
            : sys::DataMessage(MessageType::BtMessage), power(power), chanel(chanel), pattern(pattern)
        {}
    };

    class OnOff : public sys::DataMessage
    {
      public:
        /// start bluetooth
        enum class State
        {
            On,
            Off
        } state;
        OnOff(State state = State::On) : sys::DataMessage(MessageType::BtMessage), state(state)
        {}
    };

} // namespace message::bt
