cmake_minimum_required(VERSION 3.20)

project(asyncgi VERSION 0.1.0 DESCRIPTION "asyncgi - asynchronous FastCGI web application microframework")
include(GNUInstallDirs)
include(external/seal_lake)
include(external/asio)

SealLake_FindOrDownload(
        sfun 2.4.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        cmake_maintenance
)
SealLake_FindOrDownload(
        fcgi_responder 1.4.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/fcgi_responder.git
        GIT_TAG        cmake_maintenance
)
SealLake_FindOrDownload(
        whaleroute 2.0.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/whaleroute.git
        GIT_TAG        v2-params
)

set(HOT_TEACUP_OBJECT_LIB ON)
SealLake_FindOrDownload(
        hot_teacup 2.0.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/hot_teacup.git
        GIT_TAG        v2
)

set(SRC
    src/app.cpp
    src/asiodispatcher.cpp
    src/connection.cpp
    src/connectionfactory.cpp
    src/errors.cpp
    src/multithreadedruntime.cpp
    src/responsecontext.cpp
    src/responsesender.cpp
    src/runtime.cpp
    src/server.cpp
    src/testserver.cpp
    src/client.cpp
    src/taskcontext.cpp
    src/timer.cpp
    src/timerprovider.cpp
    src/connectionlistener.cpp
    src/connectionlistenerfactory.cpp
    src/clientconnection.cpp
    src/request.cpp
)

SealLake_SharedLibrary(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_17
        PROPERTIES
            CXX_EXTENSIONS OFF
        INCLUDES
            asyncgi
        BUILD_INCLUDES
            ${hot_teacup_SOURCE_DIR}/include/
            ${whaleroute_SOURCE_DIR}/include/
        LIBRARIES
           asio fcgi_responder::fcgi_responder hot_teacup::hot_teacup sfun::sfun whaleroute::whaleroute
)

SealLake_Install(
        DIRECTORIES
            ${whaleroute_SOURCE_DIR}/include/whaleroute
            ${hot_teacup_SOURCE_DIR}/include/hot_teacup
)

SealLake_OptionalBuildSteps(
        IF_ENABLED_AND_STANDALONE examples
)