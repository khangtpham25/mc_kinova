configure_file(config.in.h "${CMAKE_CURRENT_BINARY_DIR}/include/config.h")

set(kinova_SRC kinova.cpp module.cpp)
set(kinova_HDR kinova.h)

add_robot(kinova ${kinova_SRC} ${kinova_HDR})
target_include_directories(kinova PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                                          "${CMAKE_CURRENT_BINARY_DIR}/include")

add_dependencies(kinova generate-kinova-urdf)

# Optional mc_robot_tools
find_package(mc_robot_tools QUIET)
if(mc_robot_tools_FOUND)
  message(STATUS "mc_robot_tools found — enabling attached tool variants")
  target_link_libraries(kinova PRIVATE mc_robot_tools)
  target_compile_definitions(kinova PRIVATE WITH_MC_ROBOT_TOOLS)
else()
  message(STATUS "mc_robot_tools NOT found — only base Kinova variants enabled")
endif()
