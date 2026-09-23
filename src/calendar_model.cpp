#include "calendar_model.h"

#include <QLocale>

CalendarModel::CalendarModel(QObject* parent)
    : QAbstractTableModel{ parent }
{
    setMonth(QDate::currentDate());
}

int CalendarModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ROW_COUNT;
}

int CalendarModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

QVariant CalendarModel::data(const QModelIndex& index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid))
        return {};

    const Day& day{ _days[toOffset(index)] };

    switch (role)
    {
    case Qt::DisplayRole:
        return day.date.day();
    case Qt::ToolTipRole:
    {
        QStringList captions;
        for (const auto& event : day.events)
            captions << event.caption;
        return captions.join('\n');
    }
    case DateRole:
        return day.date;
    case IsCurrentMonthRole:
        return day.date.month() == _month.month();
    case IsTodayRole:
        return day.date == QDate::currentDate();
    case EventsRole:
        return QVariant::fromValue(day.events);
    default:
        return {};
    }
}

QVariant CalendarModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QLocale{}.standaloneDayName(section + 1, QLocale::LongFormat); // 1 = понедельник

    return QAbstractTableModel::headerData(section, orientation, role);
}

QModelIndex CalendarModel::indexForDate(QDate date) const
{
    const auto offset{ firstVisibleDate().daysTo(date) };
    if (offset < 0 || offset >= CELL_COUNT)
        return {};

    return index(static_cast<int>(offset) / COLUMN_COUNT, static_cast<int>(offset) % COLUMN_COUNT);
}

void CalendarModel::setMonth(QDate date)
{
    if (!date.isValid())
        return;

    const QDate month{ date.year(), date.month(), 1 };
    if (month == _month)
        return;

    // Поиск дня, с которого начинается сетка
    const QDate first{ month.addDays(1 - month.dayOfWeek()) };

    beginResetModel();
    _month = month;
    for (std::size_t i{ 0 }; i < _days.size(); ++i)
        _days[i] = Day{ first.addDays(static_cast<qint64>(i)), {} };
    endResetModel();

    emit monthChanged(_month);
    changePeriod(firstVisibleDate(), lastVisibleDate());
}

void CalendarModel::setEvents(qint64 requestIndex, const QList<Event>& events)
{
    // Отсеиваем устаревшие запросы
    if (requestIndex != _requestIndex)
    {
        qDebug() << "Отклонен запрос: " << requestIndex;
        return;
    }

    for (auto& day : _days)
        day.events.clear();

    for (const auto& event : events)
    {
        const auto offset{ firstVisibleDate().daysTo(event.date) };
        if (offset >= 0 && offset < CELL_COUNT)
            _days[static_cast<std::size_t>(offset)].events.append(event);
    }

    emit dataChanged(index(0, 0), index(ROW_COUNT - 1, COLUMN_COUNT - 1),
                     { EventsRole, Qt::ToolTipRole });
}
