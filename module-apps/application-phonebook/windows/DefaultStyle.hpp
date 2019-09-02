#pragma once

#include <Rect.hpp>
#include <stdint.h>

namespace gui::style {

static const uint32_t default_radius = 11;
static const uint32_t default_non_focus_border =1;
static const uint32_t default_focus_border =3;
static const uint32_t default_label_h = 50;

inline void round(Rect *l) {
    l->setRadius(default_radius);
    l->setPenWidth(default_non_focus_border);
    l->setPenFocusWidth(default_focus_border);
}

// monospace
inline uint32_t letter_w() { return 20; }

};
