#pragma once

#include <vector>
#include <string>
#include <algorithm>

namespace cfd {

enum class FieldType {
    SCALAR,
    VECTOR,
    TENSOR
};

/**
 * @brief Field class for storing scalar, vector, or tensor data on mesh cells
 */
class Field {
public:
    std::string name;
    FieldType type;
    std::vector<double> data;
    
    Field(const std::string& name_, FieldType type_, int size);
    
    // Data access
    double& operator()(int cellId, int component = 0);
    const double& operator()(int cellId, int component = 0) const;
    
    // Size queries
    int getNumComponents() const;
    int getSize() const { return static_cast<int>(data.size()) / getNumComponents(); }
    
    // Field operations
    void fill(double value);
    void fillComponent(int component, double value);
    void scale(double factor);
    void add(const Field& other);
    void subtract(const Field& other);
    
    // Statistics
    double min() const;
    double max() const;
    double mean() const;
    double minComponent(int component) const;
    double maxComponent(int component) const;
    
    // Bounds checking
    void clamp(double minVal, double maxVal);
    void clampComponent(int component, double minVal, double maxVal);
    
    // Copy and assignment
    Field(const Field& other);
    Field& operator=(const Field& other);
    
    // Validation
    bool isValid() const;
    bool hasNaN() const;
    bool hasInf() const;
};

} // namespace cfd
