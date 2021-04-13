cmake_minimum_required(VERSION 2.8)
project(TinyServer)

include(cmake/utils.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -o0 -ggdb -std=c++11 -lpthread -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

set(LIB_SRC
    src/fiber.cpp
    src/log.cpp
    src/util.cpp
    src/config.cpp
    src/scheduler.cpp
    src/thread.cpp
    src/iomanager.cpp
    src/timer.cpp
    )

include_directories(${PROJECT_SOURCE_DIR}/src 
                    ${PROJECT_SOURCE_DIR}/vendor/yaml-cpp/include)

link_directories(${PROJECT_SOURCE_DIR}/vendor/yaml-cpp/build)


add_library(TinyServer SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(TinyServer)

add_executable(test tests/test.cpp)
add_dependencies(test TinyServer)
force_redefine_file_macro_for_sources(test)
target_link_libraries(test TinyServer yaml-cpp pthread)

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config TinyServer)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config TinyServer yaml-cpp pthread)

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread TinyServer)
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread TinyServer yaml-cpp pthread)

add_executable(test_util tests/test_util.cpp)
add_dependencies(test_util TinyServer)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util TinyServer yaml-cpp pthread)

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber TinyServer)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber TinyServer yaml-cpp pthread)

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler TinyServer)
force_redefine_file_macro_for_sources(test_scheduler)
target_link_libraries(test_scheduler TinyServer yaml-cpp pthread)

add_executable(test_iomanager tests/test_iomanager.cpp)
add_dependencies(test_iomanager TinyServer)
force_redefine_file_macro_for_sources(test_iomanager)
target_link_libraries(test_iomanager TinyServer yaml-cpp pthread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
