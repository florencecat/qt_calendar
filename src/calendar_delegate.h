#ifndef CALENDAR_DELEGATE_H
#define CALENDAR_DELEGATE_H

#include <QStyledItemDelegate>

// Рисует одну плитку дня. Всё, что нужно, берёт из модели через роли.
class CalendarDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    static constexpr int PLATE_RADIUS{ 16 };
};

#endif // CALENDAR_DELEGATE_H
