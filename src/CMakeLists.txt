cmake_minimum_required(VERSION 3.19)
project(calendar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(calendar
    WIN32 MACOSX_BUNDLE
    main.cpp
    calendar.cpp
    calendar.h

    calendar_view.h calendar_view.cpp
    event.h
    calendar_model.h calendar_model.cpp
    calendar_delegate.h calendar_delegate.cpp
    db_worker.h db_worker.cpp
)

target_link_libraries(calendar
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

install(TARGETS calendar
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET calendar
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
