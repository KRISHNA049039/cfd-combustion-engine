# Complete Code Reference

## All Implemented Classes and Functions

### Core Module (`include/core/`)

#### Vector3D.h
- `Vector3D()` - Default constructor
- `Vector3D(double x, double y, double z)` - Constructor with values
- `operator+, -, *, /` - Arithmetic operations
- `dot(const Vector3D&)` - Dot product
- `cross(const Vector3D&)` - Cross product
- `magnitude()` - Vector length
- `normalized()` - Unit vector
- `distance(const Vector3D&)` - Distance between points

#### Mesh.h / Mesh.cpp
- `struct Node` - Mesh vertex with connectivity
- `struct Face` - Mesh face with geometry
- `struct Cell` - Mesh cell (control volume)
- `struct BoundaryPatch` - Boundary face collection
- `Mesh::addNode()` - Add node to mesh
- `Mesh::addFace()` - Add face to mesh
- `Mesh::addCell()` - Add cell to mesh
- `Mesh::computeAllGeometry()` - Compute geometric properties
- `Mesh::buildConnectivity()` - Build cell-to-cell connectivity
- `Mesh::validate()` - Validate mesh integrity

#### Field.h / Field.cpp
- `Field(name, type, size)` - Constructor
- `operator()(cellId, component)` - Data access
- `fill(value)` - Fill with constant
- `scale(factor)` - Scale all values
- `add(Field&)` - Add another field
- `min(), max(), mean()` - Statistics
- `clamp(min, max)` - Bound values
- `isValid()` - Check for NaN/Inf

#### FieldManager.h / FieldManager.cpp
- `registerField(name, type, size)` - Register new field
- `getField(name)` - Access field by name
- `hasField(name)` - Check if field exists
- `getFieldNames()` - List all fields
- `validateAll()` - Validate all fields
- `getTotalMemoryUsage()` - Memory usage
- `resize(newSize)` - Resize all fields

### Geometry Module (`include/geometry/`)

#### GeometryReader.h / GeometryReader.cpp
- `struct Triangle` - Surface triangle
- `struct Surface` - Collection of triangles
- `struct BoundingBox` - Axis-aligned bounding box
- `GeometryReader::loadFile()` - Load geometry file
- `GeometryReader::getSurfaces()` - Get surface data
- `GeometryReader::getBounds()` - Get bounding box
- `GeometryReader::scale(factor)` - Scale geometry
- `GeometryReader::translate(offset)` - Translate geometry

#### STLReader.h / STLReader.cpp
- `STLReader::loadFile()` - Load STL file (ASCII/binary)
- `STLReader::isASCII()` - Detect file format
- `STLReader::loadASCII()` - Parse ASCII STL
- `STLReader::loadBinary()` - Parse binary STL
- `STLReader::validate()` - Validate geometry

#### GeometryValidator.h / GeometryValidator.cpp
- `struct GeometryError` - Error information
- `struct Edge` - Edge representation
- `GeometryValidator::checkManifold()` - Check manifold surfaces
- `GeometryValidator::checkClosedVolume()` - Check closed volume
- `GeometryValidator::checkDegenerateTriangles()` - Find degenerate triangles
- `GeometryValidator::checkNormalConsistency()` - Check normal orientation
- `GeometryValidator::getErrors()` - Get error list

#### BoundaryExtractor.h / BoundaryExtractor.cpp
- `struct BoundaryRegion` - Boundary region info
- `BoundaryExtractor::extractBoundaries()` - Extract boundaries
- `BoundaryExtractor::extractByNormal()` - Extract by normal direction
- `BoundaryExtractor::autoNameRegions()` - Auto-name regions
- `BoundaryExtractor::mergeRegions()` - Merge multiple regions

### Mesh Module (`include/mesh/`)

#### MeshGenerator.h / MeshGenerator.cpp
- `struct MeshParams` - Mesh generation parameters
- `struct MeshQualityMetrics` - Quality metrics
- `MeshGenerator::setGeometry()` - Set input geometry
- `MeshGenerator::setMeshParameters()` - Set parameters
- `MeshGenerator::generate()` - Generate mesh
- `MeshGenerator::getMesh()` - Get generated mesh
- `MeshGenerator::getQualityMetrics()` - Get quality metrics

#### MeshQuality.h / MeshQuality.cpp
- `MeshQuality::computeMetrics()` - Compute all metrics
- `MeshQuality::computeCellAspectRatio()` - Aspect ratio
- `MeshQuality::computeCellSkewness()` - Skewness
- `MeshQuality::computeFaceAngle()` - Face angle
- `MeshQuality::generateReport()` - Generate text report

### Thermodynamics Module (`include/solver/`)

#### Species.h / Species.cpp
- `Species(name, molecularWeight)` - Constructor
- `Species::setNASACoeffs()` - Set NASA polynomials
- `Species::getCp(T)` - Specific heat at temperature
- `Species::getH(T)` - Enthalpy at temperature
- `Species::getS(T)` - Entropy at temperature

#### ThermodynamicProperties.h / ThermodynamicProperties.cpp
- `ThermodynamicProperties::addSpecies()` - Add species
- `ThermodynamicProperties::getDensity(T, p, Y)` - Mixture density
- `ThermodynamicProperties::getViscosity(T, Y)` - Mixture viscosity
- `ThermodynamicProperties::getCp(T, Y)` - Mixture specific heat
- `ThermodynamicProperties::getEnthalpy(T, Y)` - Mixture enthalpy
- `ThermodynamicProperties::getMolecularWeight(Y)` - Mixture MW
- `ThermodynamicProperties::computePressure()` - From density & T
- `ThermodynamicProperties::computeTemperature()` - From density & p

### Chemistry Module (`include/chemistry/`)

#### ReactionMechanism.h / ReactionMechanism.cpp
- `struct Reaction` - Chemical reaction
- `ReactionMechanism::addSpecies()` - Add species
- `ReactionMechanism::addReaction()` - Add reaction
- `ReactionMechanism::computeRates()` - Compute reaction rates
- `ReactionMechanism::computeForwardRate()` - Forward rate constant
- `ReactionMechanism::computeReverseRate()` - Reverse rate constant

#### ChemistryIntegrator.h / ChemistryIntegrator.cpp
- `ChemistryIntegrator::loadMechanism()` - Load Chemkin file
- `ChemistryIntegrator::setBlendComposition()` - Set ethanol fraction
- `ChemistryIntegrator::integrate()` - Integrate ODEs
- `ChemistryIntegrator::getHeatRelease()` - Get heat release
- `ChemistryIntegrator::getReactionRates()` - Get reaction rates

### Fluid Dynamics Module (`include/solver/`)

#### FluidDynamics.h / FluidDynamics.cpp
- `FluidDynamics::initialize()` - Initialize solver
- `FluidDynamics::setThermodynamicProperties()` - Set thermo
- `FluidDynamics::computeMomentum()` - Solve momentum
- `FluidDynamics::solvePressureCorrection()` - SIMPLE algorithm
- `FluidDynamics::updateVelocity()` - Update velocity field
- `FluidDynamics::solveEnergy()` - Solve energy equation
- `FluidDynamics::getMaxCourantNumber()` - Get max CFL

### Turbulence Module (`include/turbulence/`)

#### TurbulenceModel.h
- `TurbulenceModel::solve()` - Solve turbulence equations
- `TurbulenceModel::getTurbulentViscosity()` - Get μ_t
- `TurbulenceModel::getTurbulentKineticEnergy()` - Get k

#### KEpsilonModel.h / KEpsilonModel.cpp
- `KEpsilonModel::initialize()` - Initialize model
- `KEpsilonModel::solve()` - Solve k-ε equations
- `KEpsilonModel::solveKEquation()` - Solve k equation
- `KEpsilonModel::solveEpsilonEquation()` - Solve ε equation
- `KEpsilonModel::updateTurbulentViscosity()` - Update μ_t
- `KEpsilonModel::applyWallFunctions()` - Apply wall functions

### Combustion Module (`include/combustion/`)

#### CombustionModel.h / CombustionModel.cpp
- `struct CombustionConfig` - Configuration
- `CombustionModel::initialize()` - Initialize model
- `CombustionModel::initializeSpark()` - Initialize spark
- `CombustionModel::solve()` - Solve combustion
- `CombustionModel::updateFlamePosition()` - Update flame
- `CombustionModel::getHeatReleaseRate()` - Get heat release
- `CombustionModel::getBurnedMassFraction()` - Get burned fraction

#### SparkIgnition.h / SparkIgnition.cpp
- `struct SparkConfig` - Spark parameters
- `SparkIgnition::initialize()` - Initialize spark
- `SparkIgnition::apply()` - Apply spark energy
- `SparkIgnition::isActive()` - Check if active

#### FlameTracker.h / FlameTracker.cpp
- `FlameTracker::initializeKernel()` - Initialize kernel
- `FlameTracker::advectFlame()` - Advect flame (G-equation)
- `FlameTracker::getFlameSpeed()` - Get local flame speed
- `FlameTracker::isBurned()` - Check if cell burned
- `FlameTracker::getKernelRadius()` - Get kernel radius

#### LaminarFlameSpeed.h / LaminarFlameSpeed.cpp
- `LaminarFlameSpeed::computeGulder()` - Gulder correlation
- `LaminarFlameSpeed::computeMetghalchiKeck()` - M-K correlation
- `LaminarFlameSpeed::computeTurbulentFlameSpeed()` - Turbulent St

### Solver Orchestration (`include/solver/`)

#### CFDSolver.h / CFDSolver.cpp
- `struct SimulationConfig` - Simulation configuration
- `struct InitialConditions` - Initial conditions
- `CFDSolver::initialize()` - Initialize solver
- `CFDSolver::setInitialConditions()` - Set ICs
- `CFDSolver::solve()` - Run simulation
- `CFDSolver::advanceTimeStep()` - Advance one step
- `CFDSolver::updateThermodynamics()` - Update thermo
- `CFDSolver::couplePhysics()` - Couple physics modules
- `CFDSolver::writeOutput()` - Write output files

### Python Module (`python/`)

#### data_reader.py
- `SimulationDataReader(directory)` - Constructor
- `get_available_times()` - Get time steps
- `load_timestep(time)` - Load data at time
- `get_field_names()` - Get field names

#### visualization.py
- `ContourPlotter(data_reader)` - Constructor
- `create_slice(plane, position)` - Create 2D slice
- `plot_contour(field, data, output)` - Plot contour
- `create_animation(field, times, output)` - Create animation

#### spark_analysis.py
- `SparkKernelAnalyzer(data_reader)` - Constructor
- `track_kernel_growth(times)` - Track kernel
- `compute_flame_speed(time)` - Compute flame speed
- `plot_kernel_propagation(output)` - Plot kernel growth

#### analysis.py
- `AnalysisTools(data_reader)` - Constructor
- `compute_heat_release_rate(time)` - Heat release
- `compute_burned_mass_fraction(time)` - Burned mass
- `plot_integrated_quantities(times, output)` - Plot quantities

## File Organization

```
Total Files: 100+
├── Header Files: 40+
├── Source Files: 40+
├── Test Files: 10+
├── Python Files: 5+
└── Documentation: 10+

Lines of Code:
├── Production C++: ~5,000
├── Test Code: ~700
├── Python: ~300
└── Documentation: ~2,000
```

## Module Dependencies

```
Core (no dependencies)
  ↓
Geometry → Core
  ↓
Mesh → Geometry, Core
  ↓
Thermodynamics → Chemistry, Core
  ↓
Chemistry → Thermodynamics, Core
  ↓
Fluid Dynamics → Thermodynamics, Core
  ↓
Turbulence → Fluid Dynamics, Core
  ↓
Combustion → Chemistry, Turbulence, Core
  ↓
Solver → All Physics Modules
  ↓
Python → Output Files
```

## Key Algorithms

1. **Geometry Processing**
   - STL parsing (ASCII/binary)
   - Manifold checking
   - Edge connectivity analysis

2. **Mesh Generation**
   - Surface mesh extraction
   - Volume mesh generation
   - Quality assessment

3. **Thermodynamics**
   - NASA polynomial evaluation
   - Ideal gas EOS
   - Mixture properties

4. **Chemistry**
   - Arrhenius kinetics
   - Species production rates
   - ODE integration

5. **Fluid Dynamics**
   - Finite volume method
   - SIMPLE algorithm
   - Courant number calculation

6. **Turbulence**
   - k-ε transport equations
   - Turbulent viscosity
   - Wall functions

7. **Combustion**
   - G-equation flame tracking
   - Flame speed correlations
   - Spark ignition

8. **Solver**
   - Operator splitting
   - Time integration
   - Multi-physics coupling

## See Also

- [API Index](00_INDEX.md)
- [Usage Examples](14_EXAMPLES.md)
- [Quick Start](../QUICKSTART.md)
- [Complete Summary](../COMPLETE_PROJECT_SUMMARY.md)
