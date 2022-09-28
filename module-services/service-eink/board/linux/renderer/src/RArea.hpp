// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cairomm/context.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

namespace Cairo
{
    class Context;
    template <class T_CppObject>
    class RefPtr;
} // namespace Cairo

class RArea : public Gtk::DrawingArea
{
  public:
    RArea();
    virtual ~RArea();

    void setDrawData(char *data)
    {
        this->data = data;
    };

  protected:
    char *data;
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
};
