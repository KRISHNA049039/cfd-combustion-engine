# CFD Combustion Engine Simulation System

A high-performance computational fluid dynamics (CFD) simulation system for analyzing internal combustion engines with ethanol-gasoline fuel blends.

## Features

- **3D Geometry Processing**: Import CAD models (STEP, IGES, STL)
- **Mesh Generation**: Automatic tetrahedral mesh generation with boundary layers
- **Multi-Physics Simulation**:
  - Compressible Navier-Stokes equations
  - Turbulence modeling (k-epsilon, k-omega SST)
  - Detailed chemical kinetics (Chemkin format)
  - Spark ignition and flame propagation
- **Parallel Computing**: OpenMP support for multi-core execution
- **Post-Processing**: Python-based visualization and analysis tools

## Requirements

### Build Dependencies

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- Eigen3 (linear algebra)
- OpenMP (parallelization)

### Optional Dependencies

- VTK (visualization output)
- HDF5 (checkpoint files)
- CGAL or TetGen (mesh generation)
- SUNDIALS/CVODE (chemistry integration)

### Python Dependencies

- Python 3.8+
- NumPy
- Matplotlib
- PyVista or VTK
- SciPy
- Pandas
- h5py

## Building

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . -j8

# Run tests
ctest
```

## Usage

### Mesh Generation

```bash
./bin/cfd_engine --mode=mesh --config=config.json
```

### Run Simulation

```bash
./bin/cfd_engine --mode=solve --config=config.json --threads=8
```

### Post-Processing

```python
from cfd_postprocess import SimulationDataReader, ContourPlotter

# Load simulation data
reader = SimulationDataReader("output/")
times = reader.get_available_times()

# Create contour plots
plotter = ContourPlotter(reader)
plotter.plot_contour_series("temperature", times, "temp_evolution.mp4")
```

## Configuration

Create a JSON configuration file:

```json
{
  "geometry": {
    "file": "engine_geometry.stl",
    "scale": 0.001
  },
  "mesh": {
    "baseSize": 0.5,
    "boundaryLayers": 3
  },
  "simulation": {
    "startTime": 0.0,
    "endTime": 0.01,
    "timeStep": 1e-6,
    "outputInterval": 1e-4
  },
  "combustion": {
    "mechanism": "ethanol_mechanism.dat",
    "ethanolFraction": 0.15,
    "spark": {
      "location": [0, 0, 10],
      "time": 0.001,
      "energy": 50
    }
  }
}
```

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── core/              # Core data structures
│   ├── geometry/          # Geometry processing
│   ├── mesh/              # Mesh generation
│   ├── solver/            # CFD solver
│   ├── turbulence/        # Turbulence models
│   ├── combustion/        # Combustion models
│   ├── chemistry/         # Chemistry integration
│   ├── io/                # Input/output
│   ├── parallel/          # Parallel computing
│   └── boundary/          # Boundary conditions
├── src/                    # Source files
│   ├── main.cpp           # Main entry point
│   └── ...                # Implementation files
├── tests/                  # Unit tests
├── python/                 # Python post-processing
│   ├── data_reader.py
│   ├── visualization.py
│   ├── analysis.py
│   └── spark_analysis.py
└── data/                   # Example data
    ├── geometries/
    ├── mechanisms/
    └── configs/
```

## Documentation

See the `.kiro/specs/cfd-combustion-engine/` directory for detailed:
- Requirements specification
- Design document
- Implementation tasks

## License

Copyright (c) 2024. All rights reserved.

## Contributing

This is a research/educational project. Contributions welcome.

## Status

🚧 **Under Development** - Core infrastructure being implemented.

Current progress:
- [x] Project structure and build system
- [ ] Core data structures
- [ ] Geometry processing
- [ ] Mesh generation
- [ ] Fluid dynamics solver
- [ ] Turbulence modeling
- [ ] Combustion modeling
- [ ] Chemistry integration
- [ ] Parallel computing
- [ ] I/O system
- [ ] Python post-processing
