#ifndef DB_WORKER_H
#define DB_WORKER_H

#include "event.h"
#include <QObject>
class DbWorker : public QObject
{
    Q_OBJECT

public:
    ~DbWorker() override;

public slots:
    void fetchEvents(QDate from, QDate to, qint64 requestIndex);

signals:
    void eventsFetched(qint64 requestIndex, const QList<Event>& events);
    void errorOccurred(const QString& message);
    void statusUpdated(const QString& message);

private:
    bool ensureConnectionAlive();

    static constexpr auto CONNECTION{ "db_worker" };
};

#endif // DB_WORKER_H
