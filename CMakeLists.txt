cmake_minimum_required(VERSION 3.15)
project(CFD_CombustionEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(Eigen3 REQUIRED)
find_package(OpenMP REQUIRED)

# Optional packages with fallback
find_package(VTK QUIET)
find_package(HDF5 QUIET)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
)

# Source files
set(CORE_SOURCES
    src/core/Vector3D.cpp
    src/core/Mesh.cpp
    src/core/Field.cpp
    src/core/FieldManager.cpp
)

set(GEOMETRY_SOURCES
    src/geometry/GeometryReader.cpp
    src/geometry/STLReader.cpp
    src/geometry/GeometryValidator.cpp
    src/geometry/BoundaryExtractor.cpp
)

set(MESH_SOURCES
    src/mesh/MeshGenerator.cpp
    src/mesh/MeshQuality.cpp
)

set(SOLVER_SOURCES
    src/solver/CFDSolver.cpp
    src/solver/FluidDynamics.cpp
    src/solver/ThermodynamicProperties.cpp
)

set(TURBULENCE_SOURCES
    src/turbulence/TurbulenceModel.cpp
    src/turbulence/KEpsilonModel.cpp
    src/turbulence/KOmegaSSTModel.cpp
)

set(COMBUSTION_SOURCES
    src/combustion/CombustionModel.cpp
    src/combustion/SparkIgnition.cpp
    src/combustion/FlameTracker.cpp
    src/combustion/LaminarFlameSpeed.cpp
)

set(CHEMISTRY_SOURCES
    src/chemistry/ChemkinReader.cpp
    src/chemistry/ReactionMechanism.cpp
    src/chemistry/ChemistryIntegrator.cpp
    src/chemistry/Species.cpp
)

set(IO_SOURCES
    src/io/ConfigReader.cpp
    src/io/OutputWriter.cpp
    src/io/CheckpointManager.cpp
    src/io/Logger.cpp
)

set(PARALLEL_SOURCES
    src/parallel/DomainDecomposer.cpp
    src/parallel/ParallelCommunicator.cpp
)

set(BOUNDARY_SOURCES
    src/boundary/BoundaryCondition.cpp
    src/boundary/BoundaryConditionManager.cpp
)

# Create library
add_library(cfd_engine_lib STATIC
    ${CORE_SOURCES}
    ${GEOMETRY_SOURCES}
    ${MESH_SOURCES}
    ${SOLVER_SOURCES}
    ${TURBULENCE_SOURCES}
    ${COMBUSTION_SOURCES}
    ${CHEMISTRY_SOURCES}
    ${IO_SOURCES}
    ${PARALLEL_SOURCES}
    ${BOUNDARY_SOURCES}
)

target_link_libraries(cfd_engine_lib
    PUBLIC
        Eigen3::Eigen
        OpenMP::OpenMP_CXX
)

if(VTK_FOUND)
    target_link_libraries(cfd_engine_lib PUBLIC ${VTK_LIBRARIES})
    target_compile_definitions(cfd_engine_lib PUBLIC HAS_VTK)
endif()

if(HDF5_FOUND)
    target_link_libraries(cfd_engine_lib PUBLIC ${HDF5_LIBRARIES})
    target_include_directories(cfd_engine_lib PUBLIC ${HDF5_INCLUDE_DIRS})
    target_compile_definitions(cfd_engine_lib PUBLIC HAS_HDF5)
endif()

# Main executable
add_executable(cfd_engine src/main.cpp)
target_link_libraries(cfd_engine PRIVATE cfd_engine_lib)

# Testing
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS cfd_engine cfd_engine_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY python/ DESTINATION share/cfd_engine/python)
install(DIRECTORY data/ DESTINATION share/cfd_engine/data)

# Print configuration summary
message(STATUS "")
message(STATUS "CFD Combustion Engine Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  OpenMP: ${OpenMP_CXX_VERSION}")
message(STATUS "  Eigen3: ${EIGEN3_VERSION}")
message(STATUS "  VTK: ${VTK_FOUND}")
message(STATUS "  HDF5: ${HDF5_FOUND}")
message(STATUS "")
