set(CMAKE_PREFIX_PATH ${QTDIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*[.cpp|.hpp])

add_executable(MonotoneCubicSplineApp ${SOURCES})

target_link_libraries(MonotoneCubicSplineApp PRIVATE Qt6::Widgets MonotoneCubicSpline)

source_group("" FILES ${SOURCES})
