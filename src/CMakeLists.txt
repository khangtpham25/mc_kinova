configure_file(config.in.h "${CMAKE_CURRENT_BINARY_DIR}/include/config.h")

set(kinova_SRC kinova.cpp module.cpp)
set(kinova_HDR kinova.h)

add_robot(kinova ${kinova_SRC} ${kinova_HDR})
target_include_directories(kinova PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                                          "${CMAKE_CURRENT_BINARY_DIR}/include")

add_dependencies(kinova generate-kinova-urdf)
