#include <magic_enum.hpp>
#include "hal/key_input/KeyEventDefinitions.hpp"

const char *c_str(bsp::KeyCodes code)
{
    return magic_enum::enum_name(code).data();
}
