file (GLOB_RECURSE c_orbit_propagator *)

add_library(c_orbit_propagator  SHARED  ${c_orbit_propagator})

set_target_properties(c_orbit_propagator PROPERTIES
			VERSION 0.1
			SUBVERSION 1)

target_link_libraries(c_orbit_propagator)
