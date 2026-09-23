#include "calendar.h"
#include "calendar_model.h"
#include "calendar_view.h"
#include "db_worker.h"

#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QToolBar>


Calendar::Calendar(QWidget *parent)
    : QMainWindow(parent)
    , _model{ new CalendarModel{ this } }
    , _calendar{ new CalendarView{ this } }
    , _monthLabel{ new QLabel{ this } }
{
    // Настройка модели данных
    _calendar->setModel(_model);
    setCentralWidget(_calendar);
    setupToolBar();

    connect(_model, &CalendarModel::monthChanged, this, &Calendar::updateMonthLabel);

    updateMonthLabel(_model->month());

    // Настройка потока воркера
    _dbThread = new QThread(this);
    auto worker{ new DbWorker{} };
    worker->moveToThread(_dbThread);

    connect(_model, &CalendarModel::periodChanged, worker, &DbWorker::fetchEvents);
    connect(worker, &DbWorker::errorOccurred, this, &Calendar::handleMessage);
    connect(worker, &DbWorker::statusUpdated, this, &Calendar::handleMessage);
    connect(worker, &DbWorker::eventsFetched, _model, &CalendarModel::setEvents);
    connect(_dbThread, &QThread::finished, worker, &QObject::deleteLater);

    _dbThread->start();
    _model->refresh(); // вызов periodChanged -> запуск задачи воркера

    resize(900, 650);
}

void Calendar::setupToolBar()
{
    auto toolBar{ addToolBar(tr("Навигация")) };
    toolBar->setMovable(false);
    toolBar->addAction(tr("<"), _model, &CalendarModel::previousMonth);
    toolBar->addAction(tr("Сегодня"), this, [this] { _calendar->selectDate(QDate::currentDate()); });
    toolBar->addAction(tr(">"), _model, &CalendarModel::nextMonth);
    toolBar->addSeparator();

    QFont font{ _monthLabel->font() };
    font.setPointSize(font.pointSize() + 4);
    font.setBold(true);
    _monthLabel->setFont(font);
    toolBar->addWidget(_monthLabel);
}

void Calendar::updateMonthLabel(QDate month)
{
    QString name{ QLocale{}.standaloneMonthName(month.month()) };
    if (!name.isEmpty())
        name[0] = name[0].toUpper();
    _monthLabel->setText(QStringLiteral("%1 %2").arg(name).arg(month.year()));
}