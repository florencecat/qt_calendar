#include "calendar_view.h"
#include "calendar_delegate.h"
#include "calendar_model.h"

#include <QHeaderView>
#include <QMessageBox>

CalendarView::CalendarView(QWidget* parent)
    : QTableView{ parent }
{
    setItemDelegate(new CalendarDelegate{ this });

    // Плитки растягиваются на всё окно
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->hide();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setShowGrid(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setMouseTracking(true);

    connect(this, &QAbstractItemView::activated, this, [this](const QModelIndex& index) {
        emit dateActivated(index.data(CalendarModel::DateRole).toDate());
    });
}

void CalendarView::setModel(QAbstractItemModel* model)
{
    if (model && !qobject_cast<CalendarModel*>(model))
    {
        qWarning("CalendarView: only CalendarModel is supported");
        return;
    }
    QTableView::setModel(model);
}

QDate CalendarView::selectedDate() const
{
    return currentIndex().data(CalendarModel::DateRole).toDate();
}

void CalendarView::selectDate(QDate date)
{
    auto calendarModel{ qobject_cast<CalendarModel*>(model()) };
    if (!calendarModel)
        return;

    calendarModel->setMonth(date);                       // перелистнёт, если нужно
    setCurrentIndex(calendarModel->indexForDate(date));
}

void CalendarView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QTableView::currentChanged(current, previous);
    if (current.isValid())
        emit dateSelected(current.data(CalendarModel::DateRole).toDate());
}
