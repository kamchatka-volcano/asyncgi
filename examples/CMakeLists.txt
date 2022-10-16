cmake_minimum_required(VERSION 3.18)

file(GLOB SRC_FILES "*.cpp")
foreach(SRC_FILE ${SRC_FILES})
    SealLake_StringAfterLast(${SRC_FILE} "/" TEST_NAME)
    SealLake_StringBeforeLast(${TEST_NAME} "." TEST_NAME)

    SealLake_Executable(
        NAME ${TEST_NAME}
        SOURCES ${SRC_FILE}
        COMPILE_FEATURES cxx_std_17
        PROPERTIES
            CXX_EXTENSIONS OFF
        LIBRARIES
            asyncgi::asyncgi Threads::Threads
    )
    if (TEST_NAME STREQUAL example_response_dispatching_asio_task OR
        TEST_NAME STREQUAL example_asio_dispatcher)
        target_link_libraries(${TEST_NAME} PRIVATE asio)
    endif()
endforeach()