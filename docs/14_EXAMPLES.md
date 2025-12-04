# Usage Examples

## Complete Workflow Examples

### Example 1: Basic Simulation Setup

```cpp
#include "core/Mesh.h"
#include "core/FieldManager.h"
#include "solver/CFDSolver.h"
#include "geometry/STLReader.h"
#include "mesh/MeshGenerator.h"

int main() {
    // 1. Load geometry
    cfd::STLReader reader;
    reader.loadFile("engine.stl");
    auto surfaces = reader.getSurfaces();
    
    // 2. Generate mesh
    cfd::MeshGenerator meshGen;
    cfd::MeshParams params;
    params.baseSize = 1.0;
    params.numBoundaryLayers = 3;
    
    meshGen.setGeometry(surfaces);
    meshGen.setMeshParameters(params);
    meshGen.generate();
    
    cfd::Mesh mesh = meshGen.getMesh();
    
    // 3. Setup solver
    cfd::CFDSolver solver;
    cfd::SimulationConfig config;
    config.startTime = 0.0;
    config.endTime = 0.01;
    config.timeStep = 1e-6;
    
    solver.initialize(mesh, config);
    
    // 4. Set initial conditions
    cfd::InitialConditions ic;
    ic.temperature = 300.0;
    ic.pressure = 101325.0;
    ic.velocity = cfd::Vector3D(0, 0, 0);
    
    solver.setInitialConditions(ic);
    
    // 5. Run simulation
    solver.solve();
    
    return 0;
}
```

### Example 2: Mesh Generation with Quality Check

```cpp
#include "mesh/MeshGenerator.h"
#include "mesh/MeshQuality.h"
#include "geometry/STLReader.h"

void generateAndCheckMesh() {
    // Load geometry
    cfd::STLReader reader;
    reader.loadFile("geometry.stl");
    
    // Setup mesh parameters
    cfd::MeshParams params;
    params.baseSize = 0.5;
    params.boundaryLayerHeight = 0.1;
    params.growthRatio = 1.2;
    params.numBoundaryLayers = 3;
    
    // Add refinement region
    cfd::MeshParams::RefinementRegion region;
    region.center = cfd::Vector3D(0, 0, 10);
    region.radius = 5.0;
    region.size = 0.1;
    params.refinementRegions.push_back(region);
    
    // Generate mesh
    cfd::MeshGenerator meshGen;
    meshGen.setGeometry(reader.getSurfaces());
    meshGen.setMeshParameters(params);
    
    if (!meshGen.generate()) {
        std::cerr << "Mesh generation failed: " 
                  << meshGen.getLastError() << std::endl;
        return;
    }
    
    // Check quality
    cfd::MeshQuality quality;
    quality.computeMetrics(meshGen.getMesh());
    
    std::cout << quality.generateReport() << std::endl;
    
    if (quality.getNumBadCells() > 0) {
        std::cout << "Warning: " << quality.getNumBadCells() 
                  << " bad quality cells detected" << std::endl;
    }
}
```

### Example 3: Combustion Simulation with Spark Ignition

```cpp
#include "solver/CFDSolver.h"
#include "combustion/CombustionModel.h"
#include "combustion/SparkIgnition.h"

void runCombustionSimulation() {
    // Setup solver (mesh already created)
    cfd::CFDSolver solver;
    cfd::SimulationConfig config;
    config.combustionModel = "flamelet";
    config.turbulenceModel = "k-epsilon";
    
    // Initialize
    solver.initialize(mesh, config);
    
    // Setup spark ignition
    cfd::SparkConfig spark;
    spark.location = cfd::Vector3D(0, 0, 10);
    spark.ignitionTime = 0.001;  // 1 ms
    spark.ignitionEnergy = 50.0;  // 50 mJ
    spark.kernelRadius = 0.5;     // 0.5 mm
    
    // Get combustion model and initialize spark
    auto& combustion = solver.getCombustionModel();
    combustion.initializeSpark(spark, 0.0);
    
    // Run simulation
    solver.solve();
    
    // Get results
    double heatRelease = combustion.getHeatReleaseRate();
    double burnedFraction = combustion.getBurnedMassFraction();
    
    std::cout << "Heat release rate: " << heatRelease << " W" << std::endl;
    std::cout << "Burned mass fraction: " << burnedFraction << std::endl;
}
```

### Example 4: Field Operations

```cpp
#include "core/FieldManager.h"

void fieldOperationsExample() {
    cfd::FieldManager fields;
    int numCells = 1000;
    
    // Register fields
    fields.registerField("temperature", cfd::FieldType::SCALAR, numCells);
    fields.registerField("pressure", cfd::FieldType::SCALAR, numCells);
    fields.registerField("velocity", cfd::FieldType::VECTOR, numCells);
    
    // Initialize
    cfd::Field& T = fields.getField("temperature");
    cfd::Field& p = fields.getField("pressure");
    cfd::Field& U = fields.getField("velocity");
    
    T.fill(300.0);  // 300 K
    p.fill(101325.0);  // 1 atm
    
    // Set velocity field
    for (int i = 0; i < numCells; ++i) {
        U(i, 0) = 10.0;  // 10 m/s in x-direction
        U(i, 1) = 0.0;
        U(i, 2) = 0.0;
    }
    
    // Apply temperature gradient
    for (int i = 0; i < numCells; ++i) {
        T(i) = 300.0 + i * 0.1;
    }
    
    // Statistics
    std::cout << "Temperature range: " 
              << T.min() << " - " << T.max() << " K" << std::endl;
    std::cout << "Average temperature: " << T.mean() << " K" << std::endl;
    
    // Clamp values
    T.clamp(250.0, 400.0);
    
    // Validate
    if (!fields.validateAll()) {
        auto invalid = fields.getInvalidFields();
        std::cout << "Invalid fields: ";
        for (const auto& name : invalid) {
            std::cout << name << " ";
        }
        std::cout << std::endl;
    }
}
```

### Example 5: Thermodynamic Properties

```cpp
#include "solver/ThermodynamicProperties.h"
#include "chemistry/Species.h"

void thermodynamicsExample() {
    // Create thermodynamic properties
    cfd::ThermodynamicProperties thermo;
    
    // Add species (simplified)
    cfd::Species O2("O2", 32.0);  // kg/kmol
    cfd::Species N2("N2", 28.0);
    cfd::Species CO2("CO2", 44.0);
    
    // Set NASA coefficients (example values)
    std::vector<double> lowT = {3.78, -3e-3, 9.8e-6, -9.7e-9, 3.2e-12, -1.1e3, 3.7};
    std::vector<double> highT = {3.28, 1.5e-3, -3.9e-7, 5.6e-11, -2.4e-15, -1.1e3, 5.5};
    O2.setNASACoeffs(lowT, highT, 1000.0);
    
    thermo.addSpecies(O2);
    thermo.addSpecies(N2);
    thermo.addSpecies(CO2);
    
    // Compute mixture properties
    double T = 300.0;  // K
    double p = 101325.0;  // Pa
    std::vector<double> Y = {0.23, 0.77, 0.0};  // Mass fractions (air)
    
    double rho = thermo.getDensity(T, p, Y);
    double mu = thermo.getViscosity(T, Y);
    double Cp = thermo.getCp(T, Y);
    
    std::cout << "Density: " << rho << " kg/m³" << std::endl;
    std::cout << "Viscosity: " << mu << " Pa·s" << std::endl;
    std::cout << "Specific heat: " << Cp << " J/kg/K" << std::endl;
}
```

### Example 6: Chemistry Integration

```cpp
#include "chemistry/ReactionMechanism.h"
#include "chemistry/ChemistryIntegrator.h"

void chemistryExample() {
    // Create reaction mechanism
    cfd::ReactionMechanism mechanism;
    
    // Add species
    cfd::Species fuel("C2H5OH", 46.0);
    cfd::Species O2("O2", 32.0);
    cfd::Species CO2("CO2", 44.0);
    cfd::Species H2O("H2O", 18.0);
    
    mechanism.addSpecies(fuel);
    mechanism.addSpecies(O2);
    mechanism.addSpecies(CO2);
    mechanism.addSpecies(H2O);
    
    // Add reaction: C2H5OH + 3O2 -> 2CO2 + 3H2O
    cfd::Reaction rxn;
    rxn.reactants = {0, 1};  // Fuel, O2
    rxn.products = {2, 3};   // CO2, H2O
    rxn.stoichReactants = {1.0, 3.0};
    rxn.stoichProducts = {2.0, 3.0};
    rxn.A = 1e13;  // Pre-exponential factor
    rxn.beta = 0.0;
    rxn.Ea = 1.5e8;  // Activation energy J/kmol
    
    mechanism.addReaction(rxn);
    
    // Setup integrator
    cfd::ChemistryIntegrator integrator;
    integrator.setMechanism(mechanism);
    integrator.setBlendComposition(0.15);  // E15 blend
    
    // Initial conditions
    double T = 1000.0;  // K
    double p = 101325.0;  // Pa
    std::vector<double> Y = {0.1, 0.2, 0.0, 0.0};  // Mass fractions
    
    // Integrate
    double dt = 1e-6;  // 1 microsecond
    integrator.integrate(T, p, Y, dt);
    
    // Results
    double heatRelease = integrator.getHeatRelease();
    auto rates = integrator.getReactionRates();
    
    std::cout << "Heat release: " << heatRelease << " W/m³" << std::endl;
}
```

### Example 7: Turbulence Modeling

```cpp
#include "turbulence/KEpsilonModel.h"

void turbulenceExample(const cfd::Mesh& mesh) {
    cfd::FieldManager fields;
    
    // Create turbulence model
    cfd::KEpsilonModel turbModel;
    turbModel.initialize(mesh, fields);
    
    // Set initial turbulence values
    cfd::Field& k = fields.getField("k");
    cfd::Field& epsilon = fields.getField("epsilon");
    
    double U_ref = 10.0;  // Reference velocity
    double I = 0.05;      // Turbulence intensity (5%)
    double L = 0.1;       // Length scale
    
    for (int i = 0; i < mesh.getNumCells(); ++i) {
        k(i) = 1.5 * std::pow(U_ref * I, 2);
        epsilon(i) = std::pow(0.09, 0.75) * std::pow(k(i), 1.5) / L;
    }
    
    // Solve turbulence
    double dt = 1e-6;
    turbModel.solve(fields, dt);
    
    // Get turbulent viscosity
    for (int i = 0; i < mesh.getNumCells(); ++i) {
        double mu_t = turbModel.getTurbulentViscosity(i);
        double k_val = turbModel.getTurbulentKineticEnergy(i);
        // Use in flow solver
    }
}
```

### Example 8: Python Post-Processing

```python
#!/usr/bin/env python3
from cfd_postprocess import SimulationDataReader, ContourPlotter, SparkKernelAnalyzer
import numpy as np
import matplotlib.pyplot as plt

# Load simulation data
reader = SimulationDataReader("output/")
times = reader.get_available_times()

print(f"Found {len(times)} time steps")
print(f"Time range: {times[0]} - {times[-1]} s")

# Create contour plots
plotter = ContourPlotter(reader)

# Temperature contours
for time in times[::10]:  # Every 10th time step
    data = reader.load_timestep(time)
    plotter.plot_contour("temperature", data, f"temp_{time:.6f}.png")

# Create animation
plotter.create_animation("temperature", times, "temperature_evolution.mp4")

# Analyze spark kernel
analyzer = SparkKernelAnalyzer(reader)
kernel_data = analyzer.track_kernel_growth(times)

# Plot kernel radius vs time
plt.figure(figsize=(10, 6))
plt.plot(kernel_data['times'], kernel_data['radius'])
plt.xlabel('Time (s)')
plt.ylabel('Kernel Radius (mm)')
plt.title('Spark Kernel Growth')
plt.grid(True)
plt.savefig('kernel_growth.png', dpi=300)

# Compute flame speed
flame_speeds = []
for time in times:
    speed = analyzer.compute_flame_speed(time)
    flame_speeds.append(speed)

plt.figure(figsize=(10, 6))
plt.plot(times, flame_speeds)
plt.xlabel('Time (s)')
plt.ylabel('Flame Speed (m/s)')
plt.title('Flame Propagation Speed')
plt.grid(True)
plt.savefig('flame_speed.png', dpi=300)

print("Post-processing complete!")
```

## Common Patterns

### Pattern 1: Error Handling
```cpp
try {
    solver.solve();
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

### Pattern 2: Progress Monitoring
```cpp
while (currentTime < endTime) {
    solver.advanceTimeStep(dt);
    
    if (iteration % 100 == 0) {
        std::cout << "Progress: " << (currentTime/endTime)*100 << "%\r" << std::flush;
    }
}
```

### Pattern 3: Adaptive Time Stepping
```cpp
double dt = initialTimeStep;
double maxCFL = 0.5;

while (currentTime < endTime) {
    double CFL = solver.getMaxCourantNumber();
    
    if (CFL > maxCFL) {
        dt *= 0.8;  // Reduce time step
    } else if (CFL < 0.3 * maxCFL) {
        dt *= 1.2;  // Increase time step
    }
    
    solver.advanceTimeStep(dt);
}
```

## See Also

- [API Index](00_INDEX.md)
- [Configuration Guide](15_CONFIG.md)
- [Quick Start](../QUICKSTART.md)
