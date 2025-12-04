# CFD Combustion Engine - Complete API Documentation

## Documentation Index

### Core Modules
1. [Core Data Structures](01_CORE.md) - Vector3D, Mesh, Field, FieldManager
2. [Geometry Processing](02_GEOMETRY.md) - GeometryReader, STLReader, GeometryValidator, BoundaryExtractor
3. [Mesh Generation](03_MESH.md) - MeshGenerator, MeshQuality

### Physics Modules
4. [Thermodynamics](04_THERMODYNAMICS.md) - Species, ThermodynamicProperties
5. [Chemistry](05_CHEMISTRY.md) - ReactionMechanism, ChemistryIntegrator
6. [Fluid Dynamics](06_FLUID_DYNAMICS.md) - FluidDynamics solver
7. [Turbulence](07_TURBULENCE.md) - TurbulenceModel, KEpsilonModel
8. [Combustion](08_COMBUSTION.md) - CombustionModel, SparkIgnition, FlameTracker

### Solver & Infrastructure
9. [Solver Orchestration](09_SOLVER.md) - CFDSolver, time integration
10. [Boundary Conditions](10_BOUNDARY.md) - BoundaryCondition, BoundaryConditionManager
11. [Parallel Computing](11_PARALLEL.md) - DomainDecomposer, ParallelCommunicator
12. [I/O System](12_IO.md) - ConfigReader, OutputWriter, Logger

### Post-Processing
13. [Python API](13_PYTHON.md) - Data reader, visualization, analysis

### Examples & Tutorials
14. [Usage Examples](14_EXAMPLES.md) - Common use cases
15. [Configuration Guide](15_CONFIG.md) - JSON configuration reference

## Quick Links

- [Installation Guide](../QUICKSTART.md)
- [Project Summary](../COMPLETE_PROJECT_SUMMARY.md)
- [Design Document](../.kiro/specs/cfd-combustion-engine/design.md)
- [Requirements](../.kiro/specs/cfd-combustion-engine/requirements.md)

## Documentation Conventions

### Code Examples
All code examples use C++17 standard and follow the project conventions.

### Namespace
All classes are in the `cfd` namespace:
```cpp
namespace cfd {
    // All project code here
}
```

### Header Files
Headers are organized by module:
```
include/
├── core/          # Core data structures
├── geometry/      # Geometry processing
├── mesh/          # Mesh generation
├── solver/        # Solver components
├── turbulence/    # Turbulence models
├── combustion/    # Combustion models
├── chemistry/     # Chemistry integration
├── io/            # Input/output
├── parallel/      # Parallel computing
└── boundary/      # Boundary conditions
```

## Getting Started

1. Read [QUICKSTART.md](../QUICKSTART.md) for build instructions
2. Review [01_CORE.md](01_CORE.md) for fundamental data structures
3. Check [14_EXAMPLES.md](14_EXAMPLES.md) for usage examples
4. Explore specific modules as needed

## Version Information

- **Version**: 1.0.0
- **C++ Standard**: C++17
- **Build System**: CMake 3.15+
- **Compiler**: GCC 9+, Clang 10+, MSVC 2019+
