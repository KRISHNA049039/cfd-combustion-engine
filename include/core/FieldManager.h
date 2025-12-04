#pragma once

#include "core/Field.h"
#include <map>
#include <memory>
#include <vector>

namespace cfd {

/**
 * @brief Manager for all field variables in the simulation
 */
class FieldManager {
public:
    FieldManager();
    
    // Field registration and access
    void registerField(const std::string& name, FieldType type, int size);
    Field& getField(const std::string& name);
    const Field& getField(const std::string& name) const;
    bool hasField(const std::string& name) const;
    
    // Field removal
    void removeField(const std::string& name);
    void clearAll();
    
    // Field queries
    std::vector<std::string> getFieldNames() const;
    std::vector<std::string> getScalarFieldNames() const;
    std::vector<std::string> getVectorFieldNames() const;
    int getNumFields() const { return static_cast<int>(fields.size()); }
    
    // Bulk operations
    void fillAll(double value);
    void scaleAll(double factor);
    
    // Validation
    bool validateAll() const;
    std::vector<std::string> getInvalidFields() const;
    
    // Memory management
    size_t getTotalMemoryUsage() const;
    void resize(int newSize);
    
private:
    std::map<std::string, std::unique_ptr<Field>> fields;
    int currentSize;
};

} // namespace cfd
