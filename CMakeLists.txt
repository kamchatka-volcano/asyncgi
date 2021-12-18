cmake_minimum_required(VERSION 3.18)

if(NOT DEFINED ASYNCGI_SUBPROJECT)
  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(ASYNCGI_SUBPROJECT OFF)
  else()
    set(ASYNCGI_SUBPROJECT ON)
  endif()
endif()

project(asyncgi VERSION 1.0.0 DESCRIPTION "asyncgi - async web application microframework")

include(external/asio)
include(external/fcgi_responder)
include(external/hot_teacup)
include(external/sfun)
include(external/whaleroute)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SRC
    src/app.cpp
    src/connection.cpp
    src/connectionfactory.cpp
    src/connectionprocessor.cpp
    src/errors.cpp
    src/multithreadedruntime.cpp
    src/response.cpp
    src/runtime.cpp
    src/server.cpp
    src/testserver.cpp
    src/timer.cpp
    src/request.cpp
    src/requestcontext.cpp)

set(PUBLIC_HEADERS
    "include/asyncgi/app.h"
    "include/asyncgi/cookie.h"
    "include/asyncgi/errors.h"
    "include/asyncgi/header.h"
    "include/asyncgi/iruntime.h"
    "include/asyncgi/query.h"
    "include/asyncgi/request.h"
    "include/asyncgi/requestprocessor.h"
    "include/asyncgi/requestrouter.h"
    "include/asyncgi/response.h"
    "include/asyncgi/responsevalue.h"
    "include/asyncgi/route.h"
    "include/asyncgi/server.h"
    "include/asyncgi/testserver.h"
    "include/asyncgi/timer.h"
    "include/asyncgi/types.h")

add_library(asyncgi SHARED ${SRC})
target_compile_features(asyncgi PUBLIC cxx_std_17)
set_target_properties(asyncgi PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(asyncgi PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(asyncgi PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(asyncgi INTERFACE include/)
target_include_directories(asyncgi PRIVATE include/)
target_include_directories(asyncgi PRIVATE src)
target_link_libraries(asyncgi PUBLIC whaleroute sfun hot_teacup fcgi_responder asio)

add_executable(example main.cpp)
target_compile_features(example PUBLIC cxx_std_17)
set_target_properties(example PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example PRIVATE asyncgi Threads::Threads)

include(GNUInstallDirs)
install(TARGETS asyncgi
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/asyncgi)
