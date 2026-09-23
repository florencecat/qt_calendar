#include "db_worker.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

DbWorker::~DbWorker()
{
    // Деструктор вызовется через deleteLater в потоке воркера — там же и закрываем
    {
        auto db{ QSqlDatabase::database(CONNECTION, false) };
        db.close();
    }
    QSqlDatabase::removeDatabase(CONNECTION);

}

void DbWorker::fetchEvents(QDate from, QDate to, qint64 requestIndex)
{
    if (!ensureConnectionAlive())
        return;

    QSqlQuery query{ QSqlDatabase::database(CONNECTION) };

    query.prepare("SELECT id, date, name, description FROM events "
                  "WHERE date BETWEEN :from AND :to ORDER BY date, id");
    query.bindValue(":from", from);
    query.bindValue(":to", to);

    if (!query.exec())
    {
        emit errorOccurred(query.lastError().text());
        return;
    }

    QList<Event> events;
    while (query.next())
    {
        events.append({ query.value(0).toLongLong(), query.value(1).toDate(),
                       query.value(2).toString(), query.value(3).toString() });
    }

    emit eventsFetched(requestIndex, events);
}

bool DbWorker::ensureConnectionAlive()
{
    if (QSqlDatabase::contains(CONNECTION))
        return QSqlDatabase::database(CONNECTION).isOpen();

    auto db{ QSqlDatabase::addDatabase("QPSQL", CONNECTION) };
    db.setHostName(qEnvironmentVariable("CALENDAR_DB_HOST", "localhost"));
    db.setPort(qEnvironmentVariable("CALENDAR_DB_PORT", "5432").toInt());
    db.setDatabaseName(qEnvironmentVariable("CALENDAR_DB_NAME", "qt_calendar"));
    db.setUserName(qEnvironmentVariable("CALENDAR_DB_USER", "postgres"));
    db.setPassword(qEnvironmentVariable("CALENDAR_DB_PASSWORD"));

    if (!db.open())
    {
        emit errorOccurred(db.lastError().text());
        QSqlDatabase::removeDatabase(CONNECTION);
        return false;
    }
    emit statusUpdated("Успешное подключение");
    return true;
}
