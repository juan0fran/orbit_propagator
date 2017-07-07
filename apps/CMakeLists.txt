file (GLOB example_sources example.cpp)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/sensor_core CACHE STRING "sensor_core dir")

add_executable(example_sources ${example_sources})

target_link_libraries(example_sources simulator c_orbit_propagator)
