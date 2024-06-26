cmake_minimum_required(VERSION 3.18)
project(asyncgi VERSION 0.6.0 DESCRIPTION "asyncgi - asynchronous FastCGI web application microframework")

include(external/seal_lake)
if (ASYNCGI_USE_BOOST_ASIO)
    find_package(Boost REQUIRED COMPONENTS system)
else()
    include(external/asio)
endif()

SealLake_Bundle(
    NAME asyncgi_sfun
    GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG v5.1.0
    DESTINATION include/asyncgi/detail/external
    DIRECTORIES include/sfun
    TEXT_REPLACEMENTS
        "namespace sfun" "namespace asyncgi::sfun"
        "SFUN_" "ASYNCGI_SFUN_"
)

set(ASYNCGI_FCGI_RESPONDER_OBJECT_LIB ON)
SealLake_Bundle(
    NAME asyncgi_fcgi_responder
    GIT_REPOSITORY https://github.com/kamchatka-volcano/fcgi_responder.git
        GIT_TAG v1.7.0
    TEXT_REPLACEMENTS
        "namespace fcgi" "namespace asyncgi::fcgi"
)

SealLake_Bundle(
        NAME asyncgi_whaleroute
        GIT_REPOSITORY https://github.com/kamchatka-volcano/whaleroute.git
        GIT_TAG v3.1.0
        DESTINATION include/asyncgi/detail/external
        DIRECTORIES include/whaleroute
        TEXT_REPLACEMENTS
        "namespace whaleroute" "namespace asyncgi::whaleroute"
        "WHALEROUTE_" "ASYNCGI_WHALEROUTE_"
)

set(ASYNCGI_HOT_TEACUP_OBJECT_LIB ON)
SealLake_Bundle(
        NAME asyncgi_hot_teacup
        GIT_REPOSITORY https://github.com/kamchatka-volcano/hot_teacup.git
        GIT_TAG v3.3.0
        WILDCARDS
        include/hot_teacup/*.h
        DESTINATION include/asyncgi/http
        TEXT_REPLACEMENTS
        "namespace http" "namespace asyncgi::http"
        "HOT_TEACUP_" "ASYNCGI_HOT_TEACUP_"
)

set(SRC
    src/asiodispatcher.cpp
    src/io.cpp
    src/client.cpp
    src/server.cpp
    src/timer.cpp
    src/asiodispatcherservice.cpp
    src/connection.cpp
    src/connectionfactory.cpp
    src/eventhandlerproxy.cpp
    src/responsecontext.cpp
    src/responsesender.cpp
    src/ioservice.cpp
    src/serverservice.cpp
    src/clientservice.cpp
    src/taskcontext.cpp
    src/timerservice.cpp
    src/timerprovider.cpp
    src/connectionlistener.cpp
    src/connectionlistenerfactory.cpp
    src/clientconnection.cpp
    src/request.cpp
    src/response.cpp
    src/routeresponsecontextaccessor.cpp
    src/serviceholder.cpp
    )

SealLake_StaticLibrary(
    SOURCES ${SRC}
    COMPILE_FEATURES cxx_std_17
    PROPERTIES
        CXX_EXTENSIONS OFF
    LIBRARIES
        asyncgi_fcgi_responder::asyncgi_fcgi_responder
        asyncgi_sfun::asyncgi_sfun
        asyncgi_hot_teacup::asyncgi_hot_teacup
)

if (ASYNCGI_USE_BOOST_ASIO)
    SealLake_AddLibraries(Boost::boost)
else()
    SealLake_AddLibraries(asio)
endif()

SealLake_OptionalSubProjects(examples)
if (ENABLE_EXAMPLES)
    add_subdirectory(test_examples)
endif()

if (ASYNCGI_USE_BOOST_ASIO)
    target_compile_definitions(asyncgi PUBLIC "ASYNCGI_USE_BOOST_ASIO")
endif()