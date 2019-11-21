#pragma once

#include "Image.hpp"
#include "Label.hpp"

namespace gui
{

class Icon : public Rect
{
  protected:
    Image *img = nullptr;
    Label *label = nullptr;

  public:
    Icon() = delete;
    // TODO: add comment
    Icon(Item *parent, const uint32_t &x, const uint32_t &y, const std::string &imgStr, const std::string &labelStr);
    virtual ~Icon() = default;
};
} // namespace gui