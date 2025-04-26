cmake_minimum_required(VERSION 3.18)
project(asyncgi VERSION 0.6.0 DESCRIPTION "asyncgi - asynchronous FastCGI web application microframework")

include(external/seal_lake)
if (ASYNCGI_USE_BOOST_ASIO OR ASYNCGI_USE_HTTP_CLIENT)
    find_package(Boost 1.66 REQUIRED COMPONENTS system)
else()
    include(external/asio)
endif()

SealLake_Bundle(
    NAME asyncgi_sfun
    GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG develop
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
        GIT_TAG dev
    TEXT_REPLACEMENTS
        "namespace fcgi" "namespace asyncgi::detail::fcgi"
)

if (ASYNCGI_USE_HTTP_CLIENT)
    set(ASYNCGI_RESTC_CPP_WITH_UNIT_TESTS OFF)
    set(ASYNCGI_RESTC_CPP_WITH_FUNCTIONALT_TESTS OFF)
    set(ASYNCGI_RESTC_CPP_USE_CPP17 ON)
    set(ASYNCGI_RESTC_CPP_INSTALL OFF)
    set(INSTALL_RAPIDJSON_HEADERS OFF)
    set(ASYNCGI_RESTC_CPP_WITH_EXAMPLES OFF)

    SealLake_Bundle(
            NAME asyncgi_restc-cpp
            GIT_REPOSITORY https://github.com/kamchatka-volcano/restc-cpp.git
            GIT_TAG dev
            TEXT_REPLACEMENTS
            "namespace restc_cpp" "namespace asyncgi::restc_cpp"
            "restc_cpp::" "asyncgi::restc_cpp::"
            "RESTC_CPP_" "ASYNCGI_RESTC_CPP_"

    )
endif ()

SealLake_Bundle(
        NAME asyncgi_whaleroute
        GIT_REPOSITORY https://github.com/kamchatka-volcano/whaleroute.git
        GIT_TAG dev
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
        GIT_TAG dev
        WILDCARDS
        include/hot_teacup/*.h
        DESTINATION include/asyncgi/http
        TEXT_REPLACEMENTS
        "namespace http" "namespace asyncgi::http"
        "HOT_TEACUP_" "ASYNCGI_HOT_TEACUP_"
)
SealLake_Copy(
        SOURCE asyncgi_hot_teacup
        WILDCARDS
        include/hot_teacup/detail/*.h
        DESTINATION include/asyncgi/http/detail
)

set(SRC
    src/asiodispatcher.cpp
    src/io.cpp
        src/fastcgi_client.cpp
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
        src/asio_context.cpp
    src/timerservice.cpp
    src/timerprovider.cpp
    src/connectionlistener.cpp
    src/connectionlistenerfactory.cpp
    src/clientconnection.cpp
        src/requestproxy.cpp
        src/responder.cpp
    src/routeresponsecontextaccessor.cpp
    src/serviceholder.cpp
        src/fastcgirequest.cpp
        src/fastcgiresponse.cpp
        src/http_client.cpp
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
if (ASYNCGI_USE_HTTP_CLIENT)
    SealLake_AddLibraries(restc-cpp::restc-cpp)
endif ()

if (ASYNCGI_USE_BOOST_ASIO OR ASYNCGI_USE_HTTP_CLIENT)
    SealLake_AddLibraries(Boost::boost)
else()
    SealLake_AddLibraries(asio)
endif()

SealLake_OptionalSubProjects(examples)
if (ENABLE_EXAMPLES)
    add_subdirectory(test_examples)
endif()

if (ASYNCGI_USE_BOOST_ASIO OR ASYNCGI_USE_HTTP_CLIENT)
    target_compile_definitions(asyncgi PUBLIC "ASYNCGI_USE_BOOST_ASIO")
endif()
if (ASYNCGI_USE_HTTP_CLIENT)
    target_compile_definitions(asyncgi PUBLIC "ASYNCGI_USE_HTTP_CLIENT")
endif ()

if (ASYNCGI_DISABLE_GLOBAL_HTTP_NAMESPACE)
    target_compile_definitions(asyncgi PUBLIC "ASYNCGI_DISABLE_GLOBAL_HTTP_NAMESPACE")
endif ()