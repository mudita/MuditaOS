// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RArea.hpp"
#include <module-bsp/board/linux/board.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <gdkmm/general.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/widget.h>
#include <glibmm/refptr.h>
#include <glib.h>

RArea::RArea() : data{nullptr}
{}

RArea::~RArea()
{}

bool RArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    if (data != nullptr) {
        Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_data(reinterpret_cast<guchar *>(data),
                                                                        Gdk::COLORSPACE_RGB,      // GdkColorspace
                                                                        false,                    // has_alpha
                                                                        8,                        // bits_per_sample
                                                                        BOARD_EINK_DISPLAY_RES_X, // width
                                                                        BOARD_EINK_DISPLAY_RES_Y, // height,
                                                                        3 * BOARD_EINK_DISPLAY_RES_X); // rowstride,

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
