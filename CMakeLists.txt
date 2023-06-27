cmake_minimum_required(VERSION 3.24)

project(omaVideo)

if (NOT DEFINED PLATFORM)
    message(FATAL_ERROR "Define PLATFORM. Must be one of: linux, efi")
endif()

set(SOURCES
    src/core/core.c
    src/format/reader.c
    src/platform/${PLATFORM}.c
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

if ("${PLATFORM}" STREQUAL "linux")
    find_package(X11 REQUIRED)
    target_include_directories(${PROJECT_NAME} PUBLIC ${X11_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
endif()
