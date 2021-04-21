cmake_minimum_required(VERSION 2.8)
project(TinyServer)

include(cmake/utils.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -o0 -ggdb -std=c++11 -lpthread -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

set(LIB_SRC
    src/address.cpp
    src/fiber.cpp
    src/log.cpp
    src/util.cpp
    src/config.cpp
    src/scheduler.cpp
    src/thread.cpp
    src/iomanager.cpp
    src/timer.cpp
    src/hook.cpp
    src/stream.cpp
    src/socket_stream.cpp
    src/fd_manager.cpp
    src/socket.cpp
    src/bytearray.cpp
    src/tcp_server.cpp
    src/http/http.cpp
    src/http/http_parser.cpp
    src/http/http_session.cpp
    src/http/http_connection.cpp
    src/http/http_server.cpp
    src/http/servlet.cpp
    src/http/http11_parser.rl.cpp
    src/http/httpclient_parser.rl.cpp
    )

include_directories(${PROJECT_SOURCE_DIR}/src 
                    ${PROJECT_SOURCE_DIR}/vendor/yaml-cpp/include)

link_directories(${PROJECT_SOURCE_DIR}/vendor/yaml-cpp/build)


add_library(TinyServer SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(TinyServer)

add_executable(test tests/test.cpp)
add_dependencies(test TinyServer)
force_redefine_file_macro_for_sources(test)
target_link_libraries(test TinyServer yaml-cpp pthread dl)

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config TinyServer)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config TinyServer yaml-cpp pthread dl)

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread TinyServer)
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread TinyServer yaml-cpp pthread dl)

add_executable(test_util tests/test_util.cpp)
add_dependencies(test_util TinyServer)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util TinyServer yaml-cpp pthread dl)

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber TinyServer)
force_redefine_file_macro_for_sources(test_fiber)
target_link_libraries(test_fiber TinyServer yaml-cpp pthread dl)

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler TinyServer)
force_redefine_file_macro_for_sources(test_scheduler)
target_link_libraries(test_scheduler TinyServer yaml-cpp pthread dl)

add_executable(test_iomanager tests/test_iomanager.cpp)
add_dependencies(test_iomanager TinyServer)
force_redefine_file_macro_for_sources(test_iomanager)
target_link_libraries(test_iomanager TinyServer yaml-cpp pthread dl)

add_executable(test_hook tests/test_hook.cpp)
add_dependencies(test_hook TinyServer)
force_redefine_file_macro_for_sources(test_hook)
target_link_libraries(test_hook TinyServer yaml-cpp pthread dl)

add_executable(test_address tests/test_address.cpp)
add_dependencies(test_address TinyServer)
force_redefine_file_macro_for_sources(test_address)
target_link_libraries(test_address TinyServer yaml-cpp pthread dl)

add_executable(test_socket tests/test_socket.cpp)
add_dependencies(test_socket TinyServer)
force_redefine_file_macro_for_sources(test_socket)
target_link_libraries(test_socket TinyServer yaml-cpp pthread dl)

add_executable(test_bytearray tests/test_bytearray.cpp)
add_dependencies(test_bytearray TinyServer)
force_redefine_file_macro_for_sources(test_bytearray)
target_link_libraries(test_bytearray TinyServer yaml-cpp pthread dl)

add_executable(test_http tests/test_http.cpp)
add_dependencies(test_http TinyServer)
force_redefine_file_macro_for_sources(test_http)
target_link_libraries(test_http TinyServer yaml-cpp pthread dl)

add_executable(test_parser tests/test_parser.cpp)
add_dependencies(test_parser TinyServer)
force_redefine_file_macro_for_sources(test_parser)
target_link_libraries(test_parser TinyServer yaml-cpp pthread dl)

add_executable(test_tcp_server tests/test_tcp_server.cpp)
add_dependencies(test_tcp_server TinyServer)
force_redefine_file_macro_for_sources(test_tcp_server)
target_link_libraries(test_tcp_server TinyServer yaml-cpp pthread dl)

add_executable(echo_server examples/echo_server.cpp)
add_dependencies(echo_server TinyServer)
force_redefine_file_macro_for_sources(echo_server)
target_link_libraries(echo_server TinyServer yaml-cpp pthread dl)

add_executable(test_http_server tests/test_http_server.cpp)
add_dependencies(test_http_server TinyServer)
force_redefine_file_macro_for_sources(test_http_server)
target_link_libraries(test_http_server TinyServer yaml-cpp pthread dl)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
