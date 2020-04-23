#pragma once

/// ellipsis setting for core/Font & Text types to use
/// by default do not ellipsis
/// by default set elision at the end
namespace gui
{
    struct Ellipsis
    {
        bool on = false;
        enum class Pos
        {
            Begin,
            End
        } pos = Pos::End;
    };
}; // namespace gui
