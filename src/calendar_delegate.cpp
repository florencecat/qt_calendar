#include "calendar_delegate.h"
#include "calendar_model.h"

#include <QPainter>

void CalendarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QPalette& palette{ option.palette };
    const QRect tile{ option.rect.adjusted(2, 2, -2, -2) };

    const bool selected{ option.state.testFlag(QStyle::State_Selected) };
    const bool hovered{ option.state.testFlag(QStyle::State_MouseOver) };
    const bool currentMonth{ index.data(CalendarModel::IsCurrentMonthRole).toBool() };
    const bool today{ index.data(CalendarModel::IsTodayRole).toBool() };
    const auto events{ index.data(CalendarModel::EventsRole).value<QList<Event>>() };

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Фон плитки
    QColor background{ palette.color(currentMonth ? QPalette::AlternateBase : QPalette::Base) };
    if (hovered)
        background = background.darker(128);

    painter->setPen(selected ? QPen{ palette.color(QPalette::Highlight), 2 } : QPen{ Qt::NoPen });
    painter->setBrush(background);
    painter->drawRoundedRect(tile, PLATE_RADIUS, PLATE_RADIUS);

    // Номер дня
    QFont font{ option.font };
    font.setBold(today);
    painter->setFont(font);

    const QFontMetrics metrics{ font };
    const int lineHeight{ metrics.height() };
    const QRect numberRect{ tile.left() + 4, tile.top() + 4, lineHeight + 6, lineHeight + 6 };

    if (today)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(palette.color(QPalette::Highlight));
        painter->drawEllipse(numberRect);
        painter->setPen(palette.color(QPalette::HighlightedText));
    }
    else
    {
        painter->setPen(palette.color(currentMonth ? QPalette::Text : QPalette::PlaceholderText));
    }
    painter->drawText(numberRect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());

    // События
    const int rowHeight{ lineHeight + 2 };
    int y{ numberRect.bottom() + 4 };
    const int capacity{ qMax(0, (tile.bottom() - 2 - y) / rowHeight) };
    const bool overflow{ events.size() > capacity };
    const int visibleCount{ overflow ? capacity - 1 : static_cast<int>(events.size()) };

    QColor eventColor{ palette.color(QPalette::Highlight) };
    eventColor.setAlpha(50);

    for (int i{ 0 }; i < visibleCount; ++i, y += rowHeight)
    {
        const QRect eventRect{ tile.left() + 4, y, tile.width() - 8, lineHeight };
        painter->setPen(Qt::NoPen);
        painter->setBrush(eventColor);
        painter->drawRoundedRect(eventRect, 3, 3);

        const QRect textRect{ eventRect.adjusted(4, 0, -4, 0) };
        painter->setPen(palette.color(QPalette::Text));
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft,
                          metrics.elidedText(events[i].caption, Qt::ElideRight, textRect.width()));
    }

    if (overflow && capacity > 0)
    {
        painter->setPen(palette.color(QPalette::PlaceholderText));
        painter->drawText(QRect{ tile.left() + 8, y, tile.width() - 16, lineHeight },
                          Qt::AlignVCenter | Qt::AlignLeft,
                          tr("+%1 more").arg(events.size() - visibleCount));
    }

    painter->restore();
}

QSize CalendarDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return { 100, 80 };
}
