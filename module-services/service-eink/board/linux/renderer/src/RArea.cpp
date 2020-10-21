// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RArea.hpp"

#include <cairomm/context.h> // for Context
#include <gdkmm/pixbuf.h>    // for Pixbuf, COLORSPACE_RGB
#include <glib.h>            // for guchar

#include "cairomm/refptr.h" // for RefPtr
#include "gdkmm/general.h"  // for set_source_pixbuf
#include "glibmm/refptr.h"  // for RefPtr
#include "gtkmm/widget.h"   // for Allocation

RArea::RArea() : data{nullptr}
{}

RArea::~RArea()
{}

bool RArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    if (data != nullptr) {
        Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_data(reinterpret_cast<guchar *>(data),
                                                                        Gdk::COLORSPACE_RGB, // GdkColorspace
                                                                        false,               // has_alpha
                                                                        8,                   // bits_per_sample
                                                                        480,                 // width
                                                                        600,                 // height,
                                                                        3 * 480);            // rowstride,

        Gtk::Allocation allocation = get_allocation();
        const int width            = allocation.get_width();
        const int height           = allocation.get_height();

        // Draw the image in the middle of the drawing area
        Gdk::Cairo::set_source_pixbuf(cr, image, (width - image->get_width()) / 2, (height - image->get_height()) / 2);

        cr->paint();
        return true;
    }
    return false;
}
