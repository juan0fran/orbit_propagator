
file (GLOB_RECURSE simulator *)

add_library(simulator  SHARED  ${simulator})

set_target_properties(simulator PROPERTIES
			VERSION 0.1
			SUBVERSION 1)

target_link_libraries(simulator)
