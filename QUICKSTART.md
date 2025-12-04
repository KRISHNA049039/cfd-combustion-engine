# CFD Combustion Engine - Quick Start Guide

## 🚀 Get Started in 5 Minutes

### Prerequisites
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install cmake g++ libeigen3-dev

# Or on macOS
brew install cmake eigen

# Or on Windows
# Install Visual Studio 2019+ with C++ support
# Install CMake from cmake.org
# Download Eigen3 from eigen.tuxfamily.org
```

### Build the Project
```bash
# Clone or navigate to project directory
cd path/to/cfd-combustion-engine

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build (use -j8 for parallel build with 8 cores)
cmake --build . -j8

# Run tests
ctest --output-on-failure
```

### Run Your First Simulation

#### 1. Check the executable
```bash
# From build directory
./bin/cfd_engine --help
```

#### 2. Use the example configuration
```bash
# Generate mesh
./bin/cfd_engine --mode=mesh --config=../data/configs/example_config.json

# Run simulation
./bin/cfd_engine --mode=solve --config=../data/configs/example_config.json --threads=4
```

#### 3. Post-process results
```bash
# Install Python dependencies
pip install numpy matplotlib pyvista scipy pandas h5py

# Run post-processing
cd ../python
python -c "
from visualization import ContourPlotter
from data_reader import SimulationDataReader

reader = SimulationDataReader('../output/')
print('Simulation data loaded!')
"
```

---

## 📁 Project Structure

```
cfd-combustion-engine/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project overview
├── QUICKSTART.md              # This file
├── COMPLETE_PROJECT_SUMMARY.md # Full documentation
│
├── include/                    # Header files
│   ├── core/                  # Mesh, Field, Vector3D
│   ├── geometry/              # STL reader, validator
│   ├── mesh/                  # Mesh generator
│   ├── solver/                # CFD solver, fluid dynamics
│   ├── turbulence/            # Turbulence models
│   ├── combustion/            # Combustion, flame tracking
│   ├── chemistry/             # Chemical kinetics
│   ├── io/                    # I/O, config, logging
│   ├── parallel/              # Parallel computing
│   └── boundary/              # Boundary conditions
│
├── src/                       # Source files
│   ├── main.cpp              # Main entry point
│   └── [mirrors include/]
│
├── tests/                     # Unit tests
│   ├── test_vector3d.cpp
│   ├── test_mesh.cpp
│   ├── test_field.cpp
│   └── test_geometry.cpp
│
├── python/                    # Post-processing
│   ├── data_reader.py
│   ├── visualization.py
│   ├── spark_analysis.py
│   └── analysis.py
│
├── data/                      # Example data
│   ├── configs/              # Configuration files
│   ├── geometries/           # CAD models
│   └── mechanisms/           # Chemical mechanisms
│
└── .kiro/specs/              # Design documentation
    └── cfd-combustion-engine/
        ├── requirements.md
        ├── design.md
        └── tasks.md
```

---

## 🎯 Common Tasks

### Create a New Simulation

1. **Prepare geometry** (STL file)
```bash
# Place your STL file in data/geometries/
cp my_engine.stl data/geometries/
```

2. **Create configuration** (JSON file)
```json
{
  "geometry": {
    "file": "data/geometries/my_engine.stl",
    "scale": 0.001
  },
  "mesh": {
    "baseSize": 1.0,
    "boundaryLayers": 3
  },
  "simulation": {
    "startTime": 0.0,
    "endTime": 0.01,
    "timeStep": 1e-6,
    "outputInterval": 1e-4
  },
  "combustion": {
    "ethanolFraction": 0.15,
    "spark": {
      "location": [0, 0, 10],
      "time": 0.001,
      "energy": 50
    }
  }
}
```

3. **Run simulation**
```bash
./bin/cfd_engine --mode=solve --config=my_config.json --threads=8
```

---

## 🧪 Running Tests

```bash
# Run all tests
cd build
ctest

# Run specific test
./bin/cfd_tests --gtest_filter=MeshTest.*

# Run with verbose output
ctest --verbose

# Run tests in parallel
ctest -j8
```

---

## 🐛 Troubleshooting

### Build Issues

**Problem**: CMake can't find Eigen3
```bash
# Solution: Set Eigen3 path
cmake -DEigen3_DIR=/path/to/eigen3 ..
```

**Problem**: OpenMP not found
```bash
# Solution: Install OpenMP
# Ubuntu: sudo apt-get install libomp-dev
# macOS: brew install libomp
```

**Problem**: C++17 not supported
```bash
# Solution: Update compiler
# GCC: sudo apt-get install g++-9
# Or specify compiler:
cmake -DCMAKE_CXX_COMPILER=g++-9 ..
```

### Runtime Issues

**Problem**: Segmentation fault
```bash
# Solution: Run with debugger
gdb ./bin/cfd_engine
(gdb) run --mode=solve --config=config.json
(gdb) backtrace
```

**Problem**: Out of memory
```bash
# Solution: Reduce mesh size in config
# Increase baseSize parameter
# Reduce number of boundary layers
```

---

## 📊 Performance Tips

### Optimize Build
```bash
# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Debug build (for development)
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Parallel Execution
```bash
# Use all available cores
./bin/cfd_engine --mode=solve --config=config.json --threads=$(nproc)

# Or specify number
./bin/cfd_engine --mode=solve --config=config.json --threads=8
```

### Memory Management
```bash
# Monitor memory usage
top -p $(pgrep cfd_engine)

# Or use htop
htop
```

---

## 📚 Next Steps

### 1. Read the Documentation
- `README.md` - Project overview
- `COMPLETE_PROJECT_SUMMARY.md` - Full technical details
- `.kiro/specs/cfd-combustion-engine/design.md` - Architecture

### 2. Explore Examples
```bash
# Check example configurations
ls data/configs/

# View example config
cat data/configs/example_config.json
```

### 3. Modify and Extend
- Add new turbulence models in `include/turbulence/`
- Implement custom boundary conditions in `include/boundary/`
- Add chemistry mechanisms in `data/mechanisms/`
- Create visualization scripts in `python/`

### 4. Run Validation Cases
```bash
# Create simple test case
./bin/cfd_engine --mode=mesh --config=data/configs/example_config.json
./bin/cfd_engine --mode=solve --config=data/configs/example_config.json
```

---

## 🆘 Getting Help

### Check Documentation
1. `COMPLETE_PROJECT_SUMMARY.md` - Complete technical reference
2. `.kiro/specs/cfd-combustion-engine/requirements.md` - Requirements
3. `.kiro/specs/cfd-combustion-engine/design.md` - Design details

### Debug Mode
```bash
# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Run with verbose output
./bin/cfd_engine --mode=solve --config=config.json --verbose
```

### Common Commands
```bash
# Check version
./bin/cfd_engine --version

# View help
./bin/cfd_engine --help

# List available modes
./bin/cfd_engine --help | grep mode
```

---

## ✅ Verification Checklist

After building, verify everything works:

- [ ] Build completes without errors
- [ ] Tests pass (`ctest`)
- [ ] Executable runs (`./bin/cfd_engine --help`)
- [ ] Example config loads
- [ ] Python imports work

```bash
# Quick verification script
cd build
cmake --build . && \
ctest && \
./bin/cfd_engine --version && \
echo "✅ All checks passed!"
```

---

## 🎓 Learning Path

### Beginner
1. Build the project
2. Run example simulation
3. Modify configuration parameters
4. Visualize results with Python

### Intermediate
5. Create custom geometry
6. Adjust mesh parameters
7. Modify initial conditions
8. Analyze convergence

### Advanced
9. Implement new turbulence model
10. Add custom boundary conditions
11. Integrate detailed chemistry
12. Optimize parallel performance

---

## 🚀 You're Ready!

The CFD combustion engine is now set up and ready to use. Start with the example cases and gradually build up to your research applications.

**Happy simulating!** 🔥

---

*For detailed technical information, see `COMPLETE_PROJECT_SUMMARY.md`*
*For architecture details, see `.kiro/specs/cfd-combustion-engine/design.md`*
