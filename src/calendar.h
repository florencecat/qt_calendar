#ifndef CALENDAR_H
#define CALENDAR_H

#include <QDate>
#include <QMainWindow>
#include <QStatusBar>
#include <QThread>

class CalendarModel;
class CalendarView;
class QLabel;

class Calendar : public QMainWindow
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = nullptr);
    ~Calendar() override
    {
        _dbThread->quit();
        _dbThread->wait();
    }

public slots:
    void handleMessage(const QString& message) { statusBar()->showMessage(message); }

private slots:
    void updateMonthLabel(QDate month);

private:
    void setupToolBar();

    CalendarModel* _model;
    CalendarView* _calendar;
    QLabel* _monthLabel;

    QThread* _dbThread;
};
#endif // CALENDAR_H
