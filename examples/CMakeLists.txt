cmake_minimum_required(VERSION 3.18)
project(asyncgi_examples)

add_executable(example_request_processor example_request_processor.cpp)
target_compile_features(example_request_processor PUBLIC cxx_std_17)
set_target_properties(example_request_processor PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_request_processor PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_hello_world example_hello_world.cpp)
target_compile_features(example_hello_world PUBLIC cxx_std_17)
set_target_properties(example_hello_world PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_hello_world PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_router example_router.cpp)
target_compile_features(example_router PUBLIC cxx_std_17)
set_target_properties(example_router PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_router PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_timer example_timer.cpp)
target_compile_features(example_timer PUBLIC cxx_std_17)
set_target_properties(example_timer PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_timer PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_route_context example_route_context.cpp)
target_compile_features(example_route_context PUBLIC cxx_std_17)
set_target_properties(example_route_context PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_route_context PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_route_specification example_route_specification.cpp)
target_compile_features(example_route_specification PUBLIC cxx_std_17)
set_target_properties(example_route_specification PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_route_specification PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_client example_client.cpp)
target_compile_features(example_client PUBLIC cxx_std_17)
set_target_properties(example_client PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_client PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_client_in_processor example_client_in_processor.cpp)
target_compile_features(example_client_in_processor PUBLIC cxx_std_17)
set_target_properties(example_client_in_processor PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_client_in_processor PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_response_wait_future example_response_wait_future.cpp)
target_compile_features(example_response_wait_future PUBLIC cxx_std_17)
set_target_properties(example_response_wait_future PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_response_wait_future PRIVATE asyncgi::asyncgi Threads::Threads)

add_executable(example_response_dispatching_asio_task example_response_dispatching_asio_task.cpp)
target_compile_features(example_response_dispatching_asio_task PUBLIC cxx_std_17)
set_target_properties(example_response_dispatching_asio_task PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_response_dispatching_asio_task PRIVATE asyncgi::asyncgi asio Threads::Threads)

add_executable(example_asio_dispatcher example_asio_dispatcher.cpp)
target_compile_features(example_asio_dispatcher PUBLIC cxx_std_17)
set_target_properties(example_asio_dispatcher PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(example_asio_dispatcher PRIVATE asyncgi::asyncgi asio Threads::Threads)