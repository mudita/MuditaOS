#include "module-apps/application-music-player/widgets/SongItem.hpp"

namespace gui
{

    using namespace musicPlayerStyle;

    SongItem::SongItem(std::string songName, std::string authorName, std::string duration)
    {
        setMinimumSize(songItem::w, songItem::h);
        setMargins(Margins(0, style::margins::small, 0, style::margins::small));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        firstHBox = new HBox(vBox, 0, 0, 0, 0);
        firstHBox->setMinimumSize(songItem::w, songItem::text_h);
        firstHBox->setMargins(Margins(0, songItem::topMargin, 0, 0));
        firstHBox->setReverseOrder(true);
        firstHBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        secondHBox = new HBox(vBox, 0, 0, 0, 0);
        secondHBox->setMinimumSize(songItem::w, songItem::text_h);
        secondHBox->setMargins(Margins(0, songItem::topMargin, 0, 0));
        secondHBox->setReverseOrder(true);
        secondHBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        durationText = new TextFixedSize(firstHBox, 0, 0, 0, 0);
        durationText->setMinimumSize(songItem::duration_w, songItem::text_h);
        durationText->setMargins(Margins(0, 0, songItem::rightMargin, 0));
        durationText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        durationText->setUnderline(false);
        durationText->setFont(style::window::font::verysmall);
        durationText->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        durationText->setEditMode(EditMode::BROWSE);
        durationText->setText(duration);

        songText = new TextFixedSize(firstHBox, 0, 0, 0, 0);
        songText->setMinimumHeight(songItem::text_h);
        songText->setMaximumWidth(songItem::w);
        songText->setMargins(Margins(songItem::leftMargin, 0, 0, 0));
        songText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        songText->setUnderline(false);
        songText->setFont(style::window::font::verysmallbold);
        songText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        songText->setEditMode(EditMode::BROWSE);
        songText->setText(songName);

        playedSong = new ImageBox(secondHBox, 0, 0, 0, 0, new Image("messages_error_W_M"));
        playedSong->setMinimumSize(songItem::duration_w, songItem::text_h);
        playedSong->setVisible(false);

        authorText = new TextFixedSize(secondHBox, 0, 0, 0, 0);
        authorText->setMinimumHeight(songItem::text_h);
        authorText->setMaximumWidth(songItem::w);
        authorText->setMargins(Margins(songItem::leftMargin, 0, 0, 0));
        authorText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        authorText->setUnderline(false);
        authorText->setFont(style::window::font::verysmall);
        authorText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        authorText->setEditMode(EditMode::BROWSE);
        authorText->setText(authorName);
    }

    auto SongItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        return true;
    }

} /* namespace gui */
