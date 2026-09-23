# Qt Calendar

Учебный месячный календарь на **C++23 + Qt 6** (Widgets). События хранятся в **PostgreSQL** и загружаются в фоновом потоке, не блокируя интерфейс.

- сетка 6×7 на базе `QAbstractTableModel` + `QTableView`
- собственная отрисовка плиток дней через `QStyledItemDelegate`
- навигация по месяцам, подсветка сегодняшнего дня, дни соседних месяцев приглушены
- события дня выводятся на плитке, лишние сворачиваются в «+N»
- работа с БД вынесена в отдельный `QThread` (паттерн worker object)

Подробнее об устройстве проекта — в [docs/architecture.md](docs/architecture.md).

## Требования

- Qt 6.5+ (модули Core, Widgets, Sql) с плагином `QPSQL`
- CMake 3.19+
- компилятор с поддержкой C++23 (проверено на MinGW 13.1 из комплекта Qt)
- PostgreSQL (проверено на 17)

## База данных

```sql
CREATE DATABASE qt_calendar;

\c qt_calendar

CREATE TABLE events (
    id          bigserial PRIMARY KEY,
    date        date      NOT NULL,
    name        text      NOT NULL,
    description text      NOT NULL DEFAULT ''
);

CREATE INDEX events_date_idx ON events (date);

-- тестовые данные
INSERT INTO events (date, name) VALUES
    (CURRENT_DATE,     'Собеседование'),
    (CURRENT_DATE,     'Созвон с командой'),
    (CURRENT_DATE + 3, 'Код-ревью');
```

## Подключение к БД

Параметры подключения читаются из переменных окружения, поэтому паролей в репозитории нет:

| Переменная             | По умолчанию  |
|------------------------|---------------|
| `CALENDAR_DB_HOST`     | `localhost`   |
| `CALENDAR_DB_PORT`     | `5432`        |
| `CALENDAR_DB_NAME`     | `qt_calendar` |
| `CALENDAR_DB_USER`     | `postgres`    |
| `CALENDAR_DB_PASSWORD` | —             |

```
localhost:5432:qt_calendar:postgres:<пароль>
```

## Сборка и запуск

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<путь к Qt>/6.x.x/<kit>
cmake --build build
```

Или просто откройте `CMakeLists.txt` в Qt Creator.

## Структура

| Файл | Назначение |
|---|---|
| `event.h` | DTO структура `Event` для событий |
| `calendar_model.*` | `CalendarModel`: сетка из 42 дней, события, навигация по месяцам |
| `calendar_delegate.*` | отрисовка плитки дня |
| `calendar_view.*` | `CalendarView`: настройка таблицы, сигналы в терминах дат |
| `db_worker.*` | `DbWorker`: запросы к PostgreSQL в фоновом потоке |
| `calendar.*` | главное окно: собирает все компоненты вместе, панель навигации |
