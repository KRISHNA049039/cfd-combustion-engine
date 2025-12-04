# Data Directory

This directory contains example data files for the CFD combustion engine simulation system.

## Structure

- `geometries/` - Example CAD geometry files (STL, STEP, IGES)
- `mechanisms/` - Chemical kinetics mechanism files (Chemkin format)
- `configs/` - Example configuration files (JSON)

## Example Files

### Geometries

Place your engine geometry files here:
- `simple_cylinder.stl` - Simplified cylinder geometry
- `engine_head.step` - Engine head CAD model

### Mechanisms

Chemical kinetics mechanisms for ethanol-gasoline blends:
- `ethanol_gasoline.dat` - Reduced mechanism for E15 blend
- `detailed_mechanism.dat` - Detailed mechanism with 100+ species

### Configurations

Example configuration files:
- `example_config.json` - Basic simulation setup
- `high_fidelity_config.json` - High-fidelity simulation with fine mesh

## Usage

Reference these files in your simulation configuration:

```json
{
  "geometry": {
    "file": "data/geometries/simple_cylinder.stl"
  },
  "combustion": {
    "mechanism": "data/mechanisms/ethanol_gasoline.dat"
  }
}
```
