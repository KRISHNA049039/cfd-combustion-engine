# Core Data Structures API Reference

## Overview
Core data structures for mesh representation, field storage, and geometric operations.

---

## Vector3D

**File**: `include/core/Vector3D.h`

3D vector class for geometric operations.

### Constructor
```cpp
Vector3D()                              // Default: (0, 0, 0)
Vector3D(double x, double y, double z)  // Initialize with values
```

### Public Members
```cpp
double x, y, z;  // Cartesian coordinates
```

### Operations
```cpp
// Arithmetic
Vector3D operator+(const Vector3D& v) const;
Vector3D operator-(const Vector3D& v) const;
Vector3D operator*(double scalar) const;
Vector3D operator/(double scalar) const;

// Compound assignment
Vector3D& operator+=(const Vector3D& v);
Vector3D& operator-=(const Vector3D& v);
Vector3D& operator*=(double scalar);

// Vector operations
double dot(const Vector3D& v) const;        // Dot product
Vector3D cross(const Vector3D& v) const;    // Cross product
double magnitude() const;                    // Length
double magnitudeSquared() const;            // Length squared
Vector3D normalized() const;                 // Unit vector
void normalize();                            // Normalize in place
double distance(const Vector3D& v) const;   // Distance to point

// Array access
double& operator[](int i);                   // i: 0=x, 1=y, 2=z
const double& operator[](int i) const;
```

### Example
```cpp
Vector3D v1(1.0, 0.0, 0.0);
Vector3D v2(0.0, 1.0, 0.0);

double dot = v1.dot(v2);           // 0.0
Vector3D cross = v1.cross(v2);     // (0, 0, 1)
double mag = v1.magnitude();        // 1.0
Vector3D sum = v1 + v2;            // (1, 1, 0)
```

---

## Mesh

**File**: `include/core/Mesh.h`

Mesh representation with nodes, faces, cells, and connectivity.

### Structures

#### Node
```cpp
struct Node {
    int id;
    Vector3D position;
    std::vector<int> connectedCells;  // Cells sharing this node
};
```

#### Face
```cpp
struct Face {
    int id;
    std::vector<int> nodeIds;
    int ownerCell;
    int neighborCell;  // -1 for boundary
    Vector3D normal;
    Vector3D centroid;
    double area;
    
    bool isBoundary() const;  // Returns true if neighborCell == -1
};
```

#### Cell
```cpp
struct Cell {
    int id;
    std::vector<int> faceIds;
    std::vector<int> neighborCells;
    Vector3D centroid;
    double volume;
    
    int getNumFaces() const;
    int getNumNeighbors() const;
};
```

#### BoundaryPatch
```cpp
struct BoundaryPatch {
    std::string name;
    std::string type;  // "wall", "inlet", "outlet", "symmetry"
    std::vector<int> faceIds;
};
```

### Mesh Class

#### Accessors
```cpp
int getNumNodes() const;
int getNumCells() const;
int getNumFaces() const;
int getNumBoundaryFaces() const;
int getNumInternalFaces() const;

const Node& getNode(int id) const;
const Cell& getCell(int id) const;
const Face& getFace(int id) const;

Node& getNode(int id);
Cell& getCell(int id);
Face& getFace(int id);
```

#### Connectivity Queries
```cpp
std::vector<int> getCellNeighbors(int cellId) const;
std::vector<int> getNodeCells(int nodeId) const;
std::vector<int> getCellFaces(int cellId) const;
```

#### Geometric Computations
```cpp
void computeFaceGeometry(int faceId);
void computeCellGeometry(int cellId);
void computeAllGeometry();
```

#### Mesh Building
```cpp
int addNode(const Vector3D& position);
int addCell(const std::vector<int>& faceIds);
int addFace(const std::vector<int>& nodeIds, int owner, int neighbor = -1);
void addBoundaryPatch(const std::string& name, const std::string& type);
void assignFaceToBoundary(int faceId, const std::string& patchName);
```

#### Connectivity Building
```cpp
void buildConnectivity();
void buildCellNeighbors();
void buildNodeCellConnectivity();
```

#### Validation
```cpp
bool validate() const;
```

### Example
```cpp
Mesh mesh;

// Add nodes
int n0 = mesh.addNode(Vector3D(0, 0, 0));
int n1 = mesh.addNode(Vector3D(1, 0, 0));
int n2 = mesh.addNode(Vector3D(0, 1, 0));

// Add face
std::vector<int> nodeIds = {n0, n1, n2};
int faceId = mesh.addFace(nodeIds, 0, -1);

// Compute geometry
mesh.computeAllGeometry();

// Query
auto neighbors = mesh.getCellNeighbors(0);
```

---

## Field

**File**: `include/core/Field.h`

Storage for scalar, vector, or tensor field data.

### Enum
```cpp
enum class FieldType {
    SCALAR,  // 1 component
    VECTOR,  // 3 components
    TENSOR   // 9 components
};
```

### Constructor
```cpp
Field(const std::string& name, FieldType type, int size);
```

### Data Access
```cpp
double& operator()(int cellId, int component = 0);
const double& operator()(int cellId, int component = 0) const;
```

### Size Queries
```cpp
int getNumComponents() const;
int getSize() const;
```

### Field Operations
```cpp
void fill(double value);
void fillComponent(int component, double value);
void scale(double factor);
void add(const Field& other);
void subtract(const Field& other);
```

### Statistics
```cpp
double min() const;
double max() const;
double mean() const;
double minComponent(int component) const;
double maxComponent(int component) const;
```

### Bounds Checking
```cpp
void clamp(double minVal, double maxVal);
void clampComponent(int component, double minVal, double maxVal);
```

### Validation
```cpp
bool isValid() const;
bool hasNaN() const;
bool hasInf() const;
```

### Example
```cpp
// Create scalar field
Field temperature("temperature", FieldType::SCALAR, 100);
temperature.fill(300.0);
temperature(50) = 350.0;

// Create vector field
Field velocity("velocity", FieldType::VECTOR, 100);
velocity(10, 0) = 1.0;  // x-component
velocity(10, 1) = 0.5;  // y-component
velocity(10, 2) = 0.0;  // z-component

// Statistics
double maxTemp = temperature.max();
double avgTemp = temperature.mean();
```

---

## FieldManager

**File**: `include/core/FieldManager.h`

Manager for all field variables in simulation.

### Field Registration
```cpp
void registerField(const std::string& name, FieldType type, int size);
Field& getField(const std::string& name);
const Field& getField(const std::string& name) const;
bool hasField(const std::string& name) const;
```

### Field Removal
```cpp
void removeField(const std::string& name);
void clearAll();
```

### Field Queries
```cpp
std::vector<std::string> getFieldNames() const;
std::vector<std::string> getScalarFieldNames() const;
std::vector<std::string> getVectorFieldNames() const;
int getNumFields() const;
```

### Bulk Operations
```cpp
void fillAll(double value);
void scaleAll(double factor);
```

### Validation
```cpp
bool validateAll() const;
std::vector<std::string> getInvalidFields() const;
```

### Memory Management
```cpp
size_t getTotalMemoryUsage() const;
void resize(int newSize);
```

### Example
```cpp
FieldManager fields;

// Register fields
fields.registerField("pressure", FieldType::SCALAR, 1000);
fields.registerField("velocity", FieldType::VECTOR, 1000);
fields.registerField("temperature", FieldType::SCALAR, 1000);

// Access fields
Field& p = fields.getField("pressure");
p.fill(101325.0);

// Query
auto scalarFields = fields.getScalarFieldNames();
size_t memory = fields.getTotalMemoryUsage();

// Validate
if (!fields.validateAll()) {
    auto invalid = fields.getInvalidFields();
    // Handle invalid fields
}
```

---

## Best Practices

1. **Vector3D**: Use for all 3D coordinates and directions
2. **Mesh**: Build connectivity after adding all elements
3. **Field**: Always validate after operations
4. **FieldManager**: Register all fields at initialization

## Performance Tips

1. Pre-allocate field sizes
2. Use `computeAllGeometry()` once after mesh building
3. Cache frequently accessed data
4. Use const references when possible

## See Also

- [Geometry Processing](02_GEOMETRY.md)
- [Mesh Generation](03_MESH.md)
- [Usage Examples](14_EXAMPLES.md)
