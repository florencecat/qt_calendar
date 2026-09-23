#ifndef EVENT_H
#define EVENT_H

#include <QDate>
#include <QList>
#include <QString>

struct Event
{
    qint64 id{ -1 };
    QDate date;
    QString caption;
    QString description;

    bool operator==(const Event&) const = default;
};

#endif // EVENT_H
