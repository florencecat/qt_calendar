#ifndef CALENDAR_MODEL_H
#define CALENDAR_MODEL_H

#include "event.h"

#include <QAbstractTableModel>
#include <QDate>

#include <array>

class CalendarModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    // Свои роли: делегат достаёт через них данные, не зная, как устроена модель
    enum Role
    {
        DateRole = Qt::UserRole + 1,
        IsCurrentMonthRole,
        IsTodayRole,
        EventsRole,
    };
    Q_ENUM(Role)

    explicit CalendarModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QDate month() const { return _month; }
    QDate firstVisibleDate() const { return _days.front().date; }
    QDate lastVisibleDate() const { return _days.back().date; }
    QModelIndex indexForDate(QDate date) const;

public slots:
    void setMonth(QDate date);
    void nextMonth() { setMonth(_month.addMonths(1)); }
    void previousMonth() { setMonth(_month.addMonths(-1)); }
    void setToday() { setMonth(QDate::currentDate()); }

    // Повторно запросить события для текущего периода
    void refresh() { changePeriod(firstVisibleDate(), lastVisibleDate()); }

    // Сюда придёт ответ от потока БД. from/to нужны, чтобы отбросить устаревший ответ
    void setEvents(qint64 requestIndex, const QList<Event>& events);

signals:
    void monthChanged(QDate month);
    void periodChanged(QDate from, QDate to, qint64 requestIndex);

private:
    struct Day
    {
        QDate date;
        QList<Event> events;
    };

    qint64 _requestIndex{ 0 };
    void changePeriod(QDate from, QDate to)
    {
        ++_requestIndex;
        emit periodChanged(from, to, _requestIndex);
    }

    static constexpr int ROW_COUNT{ 6 };
    static constexpr int COLUMN_COUNT{ 7 };
    static constexpr int CELL_COUNT{ ROW_COUNT * COLUMN_COUNT };

    static std::size_t toOffset(const QModelIndex& index)
    {
        return static_cast<std::size_t>(index.row() * COLUMN_COUNT + index.column());
    }

    QDate _month;
    std::array<Day, CELL_COUNT> _days;
};

#endif // CALENDAR_MODEL_H
