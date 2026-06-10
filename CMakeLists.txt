cmake_minimum_required(VERSION 3.22)

set(CXX_DISABLE_WERROR 1)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME mc_kinova)
set(PROJECT_VERSION 1.0.0)
project(
  ${PROJECT_NAME}
  LANGUAGES CXX
  VERSION ${PROJECT_VERSION})

include(CTest)
enable_testing()

find_package(mc_rtc REQUIRED)
find_program(XACRO xacro REQUIRED)

find_description_package(kortex_description)

# URDF generation from xacro
file(GLOB_RECURSE KINOVA_XACRO_FILES "${KORTEX_DESCRIPTION_PATH}/*.xacro")

set(KINOVA_URDF_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/urdf")
set(KINOVA_RSDF_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/rsdf")

set(KINOVA_XACRO_PATH "${CMAKE_CURRENT_BINARY_DIR}/xacro/gen3.xacro")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/xacro/gen3.in.xacro
               ${KINOVA_XACRO_PATH})

set(GENERATED_KINOVA_URDF "${CMAKE_CURRENT_BINARY_DIR}/urdf/kinova.urdf")
add_custom_command(
  OUTPUT ${GENERATED_KINOVA_URDF}
  COMMAND
    ${XACRO} -o ${GENERATED_KINOVA_URDF} -i
    ${KORTEX_DESCRIPTION_PATH}/robots/gen3.xacro dof:=7 vision:=false sim:=true
  DEPENDS ${KINOVA_XACRO_PATH} ${KINOVA_XACRO_FILES})

add_custom_target(generate-kinova-urdf ALL DEPENDS ${GENERATED_KINOVA_URDF})

add_subdirectory(src)

# Install data files
install(FILES ${GENERATED_KINOVA_URDF} DESTINATION ${KINOVA_URDF_DIR})
install(DIRECTORY rsdf/ DESTINATION ${KINOVA_RSDF_DIR})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/etc/ForceSensorCalibration")
  install(
    DIRECTORY etc/ForceSensorCalibration/
    DESTINATION ${MC_CONTROLLER_RUNTIME_INSTALL_PREFIX}/ForceSensorCalibration/
    FILES_MATCHING
    PATTERN "*.yaml")
endif()

# Tests
if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
  add_subdirectory(tests)
endif()
