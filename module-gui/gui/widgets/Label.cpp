/*
 * Label.cpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */
#include "log/log.hpp"
#include "utf8/UTF8.hpp"

#include "../core/DrawCommand.hpp"
#include "../core/Font.hpp"

#include "Label.hpp"
#include <Style.hpp>

namespace gui
{

    Label::Label()
        : Rect(), text{""}, textDisplayed{""}, charDrawableCount{0}, stringPixelWidth{0}, textColor{0, 0}, font{nullptr}
    {
        setFont(style::window::font::medium);
    }

    Label::Label(
        Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, const UTF8 &newText)
        : Rect{parent, x, y, w, h},
          text(newText), textDisplayed{""}, charDrawableCount{0}, stringPixelWidth{0}, textColor{0, 0}
    {
        setFont(style::window::font::medium);
    }

    Label::Label(Item *parent, meta::Label label) : Label(parent, label.x, label.y, label.w, label.h, label.text)
    {
        setPenFocusWidth(style::window::default_border_no_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);
        setFont(label.font);
        setEllipsis();
        setAlignment(label.align);
        setRadius(label.radius);
        setEdges(label.edges);
        setMargins(label.margins);
    }

    void Label::calculateDisplayText()
    {
        uint32_t availableSpace = widgetArea.w;
        charDrawableCount       = font->getCharCountInSpace(text, availableSpace);
        textArea.w              = font->getPixelWidth(text.substr(0, charDrawableCount));
        textDisplayed           = font->getTextWithElipsis(text, availableSpace, ellipsis);
        stringPixelWidth        = font->getPixelWidth(textDisplayed, 0, textDisplayed.length());
        textArea.h              = font->info.line_height;

        // calculate vertical position of text
        if (alignment.vertical_center) {
            textArea.y = (widgetArea.h - font->info.line_height) / 2 + font->info.base;
        }
        else if (alignment.vertical_top) {
            textArea.y = font->info.base + margins.top;
        }
        else if (alignment.vertical_bottom) {
            textArea.y = widgetArea.h - font->info.line_height + font->info.base - margins.bottom;
        }
        // calculate horizontal position o text
        textArea.x = 0;
        if (alignment.horizontal_center) {
            textArea.x = (widgetArea.w - textArea.w) / 2;
        }
        else if (alignment.horizontal_left) {
            textArea.x = margins.left;
        }
        else if (alignment.horizontal_right) {
            textArea.x = widgetArea.w - textArea.w - margins.right;
        }

        // if dots mode is disabled and line mode is enabled calculate positiona and width of the line
        if ((!ellipsis.on) && (lineMode) && (lineFront != nullptr)) {
            uint32_t spaceWidth = font->getCharPixelWidth(' ');
            int32_t lineW       = availableSpace - stringPixelWidth;
            uint32_t lineY      = textArea.y - font->getCharPixelHeight('a') / 2;
            if (lineW < 0)
                lineW = 0;

            lineFront->setVisible(true);
            lineBack->setVisible(true);
            // both lines are visible
            if (alignment.horizontal_center) {
                lineFront->setPosition(0, lineY);
                lineFront->setSize(lineW / 2 - spaceWidth, 2);
                lineBack->setPosition(lineW / 2 + stringPixelWidth + spaceWidth, lineY);
                lineBack->setSize(lineW / 2 - spaceWidth, 2);
            }
            else if (alignment.horizontal_right) {
                lineFront->setPosition(0, lineY);
                lineFront->setSize(lineW - spaceWidth, 2);
                lineBack->setVisible(false);
            }
            else if (alignment.horizontal_left) {
                lineBack->setPosition(stringPixelWidth + spaceWidth, lineY);
                lineBack->setSize(lineW - spaceWidth, 2);
                lineFront->setVisible(false);
            }
        }
    }

    void Label::setText(const UTF8 &text)
    {
        this->text = text;
        auto fits  = textFitsIn(text, area(Area::Normal).w);
        if (!fits) {
            fitTextIn(text);
        }
        calculateDisplayText();
    }

    Label::Fits Label::textFitsIn(const UTF8 &text, uint32_t width)
    {
        Fits fits;
        auto cnt            = font->getCharCountInSpace(text, area(Area::Max).w);
        fits.fits           = cnt == text.length();
        fits.space_consumed = font->getPixelWidth(text);
        return fits;
    }

    void Label::fitTextIn(const UTF8 &text)
    {
        // if (area(Item::Area::Max).w > area(Item::Area::Normal).w) {}
        Fits fits = textFitsIn(text, area(Item::Area::Max).w);
        if (fits) {
            setSize(fits.space_consumed, h());
        }
        else {
            setSize(area(Item::Area::Max).w, h());
        }
    }

    void Label::clear()
    {
        this->text = UTF8("");
        calculateDisplayText();
    }

    UTF8 Label::getText()
    {
        return text;
    }

    void Label::setAlignment(const Alignment &alignment)
    {
        this->alignment = alignment;
        calculateDisplayText();
    }

    void Label::setMargins(const Margins &margins)
    {
        this->margins = margins;
        calculateDisplayText();
    }

    void Label::setEllipsis(gui::Ellipsis::Pos pos)
    {
        ellipsis.pos = pos;
        ellipsis.on  = true;
        calculateDisplayText();
    }

    void Label::setEllipsis(gui::Ellipsis ellipsis)
    {
        this->ellipsis = ellipsis;
        calculateDisplayText();
    }

    void Label::setLineMode(const bool &val)
    {
        // if line mode is disable remove the line if it was previously created
        if (val == false) {
            if (lineFront != nullptr) {
                this->removeWidget(lineFront);
                this->removeWidget(lineBack);
                delete lineFront;
                delete lineBack;
                lineFront = nullptr;
                lineBack  = nullptr;
            }
        }
        else {
            if (lineFront == nullptr) {
                lineFront = new Rect(this, 0, 0, 0, 0);
                lineBack  = new Rect(this, 0, 0, 0, 0);
            }
        }
        calculateDisplayText();
    }

    std::list<DrawCommand *> Label::buildDrawList()
    {

        std::list<DrawCommand *> commands;

        // check if widget is visible
        if (visible == false) {
            return commands;
        }

        // get children draw commands
        std::list<DrawCommand *> commandsChildren;
        commandsChildren = Item::buildDrawList();

        // base class draw commands
        std::list<DrawCommand *> commandsBase;
        commandsBase = gui::Rect::buildDrawList();

        // set local draw commands - text command
        CommandText *textCmd = new CommandText();
        textCmd->str         = textDisplayed;
        textCmd->fontID      = font->id;
        textCmd->color       = textColor;

        textCmd->x          = drawArea.x;
        textCmd->y          = drawArea.y;
        textCmd->w          = drawArea.w;
        textCmd->h          = drawArea.h;
        textCmd->tx         = textArea.x;
        textCmd->ty         = textArea.y;
        textCmd->tw         = textArea.w;
        textCmd->th         = textArea.h;
        textCmd->charsWidth = stringPixelWidth;

        textCmd->areaX = widgetArea.x;
        textCmd->areaY = widgetArea.y;
        textCmd->areaW = widgetArea.w;
        textCmd->areaH = widgetArea.h;

        commands.splice(commands.end(), commandsBase);
        commands.push_back(textCmd);
        if (not commandsChildren.empty())
            commands.splice(commands.end(), commandsChildren);

        return commands;
    }

    void Label::setPosition(const short &x, const short &y)
    {
        gui::Rect::setPosition(x, y);
        calculateDisplayText();
    }

    void Label::setSize(const unsigned short w, const unsigned short h)
    {
        gui::Rect::setSize(w, h);
        calculateDisplayText();
    }

    void Label::setFont(const UTF8 &fontName)
    {

        uint32_t fontID = FontManager::getInstance().getFontID(fontName.c_str());
        Font *newFont   = FontManager::getInstance().getFont(fontID);
        if (newFont != nullptr) {
            font = newFont;
            calculateDisplayText();
        }
        else {
            LOG_ERROR("Font not found!");
        }
    }

    void Label::setTextColor(Color color)
    {
        textColor = color;
    }

    uint32_t Label::getTextNeedSpace()
    {
        if (font == nullptr) {
            return 0;
        }
        return font->getPixelWidth(text);
    }

} /* namespace gui */
