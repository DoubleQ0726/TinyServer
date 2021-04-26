cmake_minimum_required(VERSION 3.16)
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
    src/uri.cpp
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

set(LIBS
        TinyServer
        dl
        pthread
        yaml-cpp)

TinyServer_Add_Executable(test "tests/test.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_config "tests/test_config.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_thread "tests/test_thread.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_util "tests/test_util.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_fiber "tests/test_fiber.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_scheduler "tests/test_scheduler.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_iomanager "tests/test_iomanager.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_hook "tests/test_hook.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_address "tests/test_address.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_socket "tests/test_socket.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_bytearray "tests/test_bytearray.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_http "tests/test_http.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_parser "tests/test_parser.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_tcp_server "tests/test_tcp_server.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(echo_server "examples/echo_server.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_http_server "tests/test_http_server.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_http_connection "tests/test_http_connection.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(test_uri "tests/test_uri.cpp" TinyServer "${LIBS}")
TinyServer_Add_Executable(my_http_server "samples/my_http_server.cpp" TinyServer "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
