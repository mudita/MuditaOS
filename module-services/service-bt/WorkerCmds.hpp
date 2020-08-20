#pragma once

struct BtCmd
{
    enum class Cmd {
        Init,
        TimerPoll,
    } cmd;
};
