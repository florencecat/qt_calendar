#ifndef EVENT_VIEW_H
#define EVENT_VIEW_H

#include <QWidget>
#include <QLabel>

class EventView : public QWidget
{
    Q_OBJECT
public:
    explicit EventView(QWidget *parent = nullptr);

private:
    QLabel* _dayLabel;

signals:
};

#endif // EVENT_VIEW_H
