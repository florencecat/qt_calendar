#ifndef CALENDAR_VIEW_H
#define CALENDAR_VIEW_H

#include <QDate>
#include <QTableView>

class CalendarView : public QTableView
{
    Q_OBJECT

public:
    explicit CalendarView(QWidget* parent = nullptr);

    void setModel(QAbstractItemModel* model) override;

    QDate selectedDate() const;

public slots:
    void selectDate(QDate date);

signals:
    void dateSelected(QDate date);   // клик / стрелки
    void dateActivated(QDate date);  // двойной клик / Enter

protected:
    void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
};

#endif // CALENDAR_VIEW_H
