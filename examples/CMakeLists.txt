cmake_minimum_required(VERSION 3.18)

file(GLOB SRC_FILES "*.cpp")
foreach (SRC_FILE ${SRC_FILES})
    SealLake_StringAfterLast(${SRC_FILE} "/" EXAMPLE_NAME)
    SealLake_StringBeforeLast(${EXAMPLE_NAME} "." EXAMPLE_NAME)

    SealLake_Executable(
            NAME ${EXAMPLE_NAME}
            SOURCES ${SRC_FILE}
            COMPILE_FEATURES cxx_std_17
            PROPERTIES
            CXX_EXTENSIONS OFF
            LIBRARIES
            asyncgi::asyncgi Threads::Threads
    )
    if (EXAMPLE_NAME STREQUAL example_response_dispatching_asio_task OR
            EXAMPLE_NAME STREQUAL example_asio_dispatcher)
        if (ASYNCGI_USE_BOOST_ASIO)
            target_link_libraries(${EXAMPLE_NAME} PRIVATE Boost::boost)
        else()
            target_link_libraries(${EXAMPLE_NAME} PRIVATE asio)
        endif()
    endif ()
endforeach ()