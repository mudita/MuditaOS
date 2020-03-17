#include "AddContactIcon.hpp"
#include "i18/i18.hpp"

namespace gui
{

    AddContactIcon::AddContactIcon(Item *parent, const uint32_t &x, const uint32_t &y)
        : Icon(parent, x, y, crossImg, utils::localize.get(addContactStr))
    {}
} // namespace gui
