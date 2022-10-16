cmake_minimum_required(VERSION 3.20)

project(asyncgi VERSION 0.1.0 DESCRIPTION "asyncgi - asynchronous FastCGI web application microframework")
include(GNUInstallDirs)
include(external/seal_lake)
include(external/asio)

SealLake_Bundle(
        NAME asyncgi_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        cmake_maintenance
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace asyncgi::sfun"
            "SFUN_" "ASYNCGI_SFUN_"
)

SealLake_Bundle(
        NAME asyncgi_fcgi_responder
        GIT_REPOSITORY https://github.com/kamchatka-volcano/fcgi_responder.git
        GIT_TAG        cmake_maintenance
        TEXT_REPLACEMENTS
            "namespace fcgi" "namespace asyncgi::fcgi"
)

SealLake_Bundle(
        NAME asyncgi_whaleroute
        GIT_REPOSITORY https://github.com/kamchatka-volcano/whaleroute.git
        GIT_TAG        v2-params
        DESTINATION include/asyncgi/detail/external
        DIRECTORIES include/whaleroute
        TEXT_REPLACEMENTS
            "namespace whaleroute" "namespace asyncgi::whaleroute"
)

set(ASYNCGI_HOT_TEACUP_OBJECT_LIB ON)
SealLake_Bundle(
        NAME asyncgi_hot_teacup
        GIT_REPOSITORY https://github.com/kamchatka-volcano/hot_teacup.git
        GIT_TAG        v2
        WILDCARDS
            include/hot_teacup/*.h
        DESTINATION include/asyncgi/http
        TEXT_REPLACEMENTS
            "namespace http" "namespace asyncgi::http"
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
        INTERFACE_INCLUDES
            asyncgi/detail/external
        INCLUDES
            asyncgi/detail/external
        LIBRARIES
            asio
            asyncgi_fcgi_responder::asyncgi_fcgi_responder
            asyncgi_sfun::asyncgi_sfun
            asyncgi_hot_teacup::asyncgi_hot_teacup

)

SealLake_OptionalBuildSteps(examples)