file (GLOB example_sources example.cpp)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/sensor_core CACHE STRING "example dir")

add_executable(example_sources ${example_sources})

target_link_libraries(example_sources simulator c_orbit_propagator)

file (GLOB pass_predictor pass_predictor.cpp)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/sensor_core CACHE STRING "pass_predictor dir")

add_executable(pass_predictor ${pass_predictor})

target_link_libraries(pass_predictor simulator c_orbit_propagator)
