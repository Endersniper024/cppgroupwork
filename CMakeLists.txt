cmake_minimum_required(VERSION 3.16) # CMake install : https://cmake.org/download/
project(cppgroupwork LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "c:/Qt/6.9.0/mingw_64") # Qt Kit Dir
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# find_package(Qt6 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS
# aux_source_directory(./src srcs)

# Specify MSVC UTF-8 encoding   
# add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
# add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# add_executable(${PROJECT_NAME}
#     WIN32 # If you need a terminal for debug, please comment this statement 
#     ${srcs} 
# ) 
# target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets) # Qt5 Shared Library


# cmake_minimum_required(VERSION 3.16)
# project(CollaborativeLearningTimeManager LANGUAGES CXX)

set(CMAKE_C_COMPILER "c:/Qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "c:/Qt/Tools/mingw1310_64/bin/g++.exe")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Modules
find_package(Qt6 COMPONENTS Core Gui Widgets Sql Network REQUIRED)

# For older Qt5
# find_package(Qt5 COMPONENTS Core Gui Widgets Sql REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Project Sources
set(PROJECT_SOURCES
    src/main.cpp

    src/core/User.cpp
    src/core/DatabaseManager.cpp
    src/core/Subject.cpp
    src/core/Task.cpp
    src/core/TimeLog.cpp
    src/core/ActivityMonitorService.cpp

    src/core/reports/DataAggregator.cpp
    src/core/reports/LLMCommunicator.cpp
    src/core/reports/MetricCalculator.cpp
    src/core/reports/ReportGenerator.cpp

    src/gui/ProfileDialog.cpp
    src/gui/LoginDialog.cpp
    src/gui/MainWindow.cpp
    src/gui/SubjectDialog.cpp
    src/gui/TaskDialog.cpp
    src/gui/TimerWidget.cpp
    src/gui/RegisterDialog.cpp 

    src/gui/report/ReportDisplayDialog.cpp
    src/gui/report/ReportSettingsDialog.cpp
    # Add more source files as they are created
)

set(PROJECT_HEADERS
    src/core/User.h
    src/core/DatabaseManager.h
    src/core/Subject.h
    src/core/Task.h
    src/core/TimeLog.h
    src/core/ActivityMonitorService.h
    src/core/reports/Core.h

    src/core/reports/DataAggregator.h
    src/core/reports/IDataAggregator.h
    src/core/reports/LLMCommunicator.h
    src/core/reports/ILLMCommunicator.h
    src/core/reports/MetricCalculator.h
    src/core/reports/ReportDataStructures.h
    src/core/reports/ReportEnums.h
    src/core/reports/ReportGenerator.h
    src/core/reports/ReportParameters.h

    src/gui/ui_ProfileDialog.h
    src/gui/ui_LoginDialog.h
    src/gui/ui_MainWindow.h
    src/gui/ProfileDialog.h
    src/gui/LoginDialog.h
    src/gui/MainWindow.h
    src/gui/SubjectDialog.h
    src/gui/TaskDialog.h
    src/gui/TimerWidget.h
    src/gui/RegisterDialog.h

    src/gui/report/ReportDisplayDialog.h
    src/gui/report/ReportSettingsDialog.h
    # Add more header files
)


# Executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)

# Create a folder for the database
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
message(STATUS "Database will be created in ${CMAKE_BINARY_DIR}/data")
