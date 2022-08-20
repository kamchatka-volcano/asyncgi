cmake_minimum_required(VERSION 3.18)

project(asyncgi VERSION 0.1.0 DESCRIPTION "asyncgi - asynchronous FastCGI web application microframework")
include(GNUInstallDirs)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(ASYNCGI_SUBPROJECT OFF)
else()
  set(ASYNCGI_SUBPROJECT ON)
endif()

include(external/asio)
find_package(sfun 2.0.0 QUIET)
if (NOT sfun_FOUND)
    message("Configuration info: sfun library wasn't found on your system, downloading it from the GitHub repository")
    include (external/sfun)
endif()

find_package(whaleroute 1.0.0 QUIET)
if (NOT whaleroute_FOUND)
    message("Configuration info: whaleroute library wasn't found on your system, downloading it from the GitHub repository")
    include (external/whaleroute)
endif()

find_package(fcgi_responder 1.2.2 QUIET)
if (NOT fcgi_responder_FOUND)
    message("Configuration info: fcgi_responder library wasn't found on your system, downloading it from the GitHub repository")
    include (external/fcgi_responder)
endif()

find_package(hot_teacup 2.0.0 QUIET)
if (NOT hot_teacup_FOUND)
    message("Configuration info: hot_teacup library wasn't found on your system, downloading it from the GitHub repository")
    include (external/hot_teacup)
endif()


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SRC
    src/app.cpp
    src/connection.cpp
    src/connectionfactory.cpp
    src/errors.cpp
    src/multithreadedruntime.cpp
    src/response.cpp
    src/runtime.cpp
    src/server.cpp
    src/testserver.cpp
    src/client.cpp
    src/timer.cpp
    src/timerprovider.cpp
    src/connectionlistener.cpp
    src/connectionlistenerfactory.cpp
    src/clientconnection.cpp
    src/request.cpp
    src/requestcontext.cpp)

set(PUBLIC_HEADERS
    "include/asyncgi/app.h"
    "include/asyncgi/errors.h"
    "include/asyncgi/iapp.h"
    "include/asyncgi/iclient.h"
    "include/asyncgi/iserver.h"
    "include/asyncgi/itimer.h"
    "include/asyncgi/request.h"
    "include/asyncgi/requestprocessor.h"
    "include/asyncgi/requestrouter.h"
    "include/asyncgi/response.h"
    "include/asyncgi/testserver.h"
    "include/asyncgi/types.h")

add_library(asyncgi SHARED ${SRC})
add_library(asyncgi::asyncgi ALIAS asyncgi)

target_compile_features(asyncgi PUBLIC cxx_std_17)
set_target_properties(asyncgi PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(asyncgi PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(asyncgi PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_include_directories(
    asyncgi
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(
    asyncgi
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/asyncgi>
)
target_include_directories(
    asyncgi
    INTERFACE
    $<BUILD_INTERFACE:${whaleroute_SOURCE_DIR}/include/>
)
target_include_directories(
    asyncgi
    INTERFACE
    $<BUILD_INTERFACE:${hot_teacup_SOURCE_DIR}/include/>
)

target_include_directories(asyncgi PRIVATE include/)

target_link_libraries(asyncgi PRIVATE asio fcgi_responder::fcgi_responder hot_teacup::hot_teacup whaleroute::whaleroute sfun::sfun)

add_executable(example main.cpp)
target_compile_features(example PUBLIC cxx_std_17)
set_target_properties(example PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example PRIVATE asyncgi Threads::Threads)


option(INSTALL_ASYNCGI "Install asyncgi library unconditionally" OFF)
if (NOT ASYNCGI_SUBPROJECT OR INSTALL_ASYNCGI)
    install(TARGETS asyncgi
            EXPORT asyncgi-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/asyncgi
    )
    install(DIRECTORY ${whaleroute_SOURCE_DIR}/include/whaleroute DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/asyncgi)
    install(DIRECTORY ${hot_teacup_SOURCE_DIR}/include/hot_teacup DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/asyncgi)

    set(ASYNCGI_PACK_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/asyncgi)

    install(EXPORT asyncgi-targets
            FILE asyncgiTargets.cmake
            NAMESPACE asyncgi::
            DESTINATION ${ASYNCGI_PACK_PATH}
            )
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/asyncgiConfigVersion.cmake
            COMPATIBILITY SameMajorVersion
            ARCH_INDEPENDENT
    )
    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/asyncgiConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/asyncgiConfig.cmake
            INSTALL_DESTINATION ${ASYNCGI_PACK_PATH}
            )
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/asyncgiConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/asyncgiConfigVersion.cmake
            DESTINATION ${ASYNCGI_PACK_PATH}
            )
endif()