#include "core/FieldManager.h"
#include <stdexcept>
#include <algorithm>

namespace cfd {

FieldManager::FieldManager() : currentSize(0) {
}

void FieldManager::registerField(const std::string& name, FieldType type, int size) {
    fields[name] = std::make_unique<Field>(name, type, size);
    currentSize = size;
}

Field& FieldManager::getField(const std::string& name) {
    auto it = fields.find(name);
    if (it == fields.end()) {
        throw std::runtime_error("Field not found: " + name);
    }
    return *it->second;
}

const Field& FieldManager::getField(const std::string& name) const {
    auto it = fields.find(name);
    if (it == fields.end()) {
        throw std::runtime_error("Field not found: " + name);
    }
    return *it->second;
}

bool FieldManager::hasField(const std::string& name) const {
    return fields.find(name) != fields.end();
}

void FieldManager::removeField(const std::string& name) {
    fields.erase(name);
}

void FieldManager::clearAll() {
    fields.clear();
    currentSize = 0;
}

std::vector<std::string> FieldManager::getFieldNames() const {
    std::vector<std::string> names;
    names.reserve(fields.size());
    for (const auto& pair : fields) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> FieldManager::getScalarFieldNames() const {
    std::vector<std::string> names;
    for (const auto& pair : fields) {
        if (pair.second->type == FieldType::SCALAR) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> FieldManager::getVectorFieldNames() const {
    std::vector<std::string> names;
    for (const auto& pair : fields) {
        if (pair.second->type == FieldType::VECTOR) {
            names.push_back(pair.first);
        }
    }
    return names;
}

void FieldManager::fillAll(double value) {
    for (auto& pair : fields) {
        pair.second->fill(value);
    }
}

void FieldManager::scaleAll(double factor) {
    for (auto& pair : fields) {
        pair.second->scale(factor);
    }
}

bool FieldManager::validateAll() const {
    for (const auto& pair : fields) {
        if (!pair.second->isValid()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> FieldManager::getInvalidFields() const {
    std::vector<std::string> invalid;
    for (const auto& pair : fields) {
        if (!pair.second->isValid()) {
            invalid.push_back(pair.first);
        }
    }
    return invalid;
}

size_t FieldManager::getTotalMemoryUsage() const {
    size_t total = 0;
    for (const auto& pair : fields) {
        total += pair.second->data.size() * sizeof(double);
    }
    return total;
}

void FieldManager::resize(int newSize) {
    for (auto& pair : fields) {
        int components = pair.second->getNumComponents();
        pair.second->data.resize(newSize * components, 0.0);
    }
    currentSize = newSize;
}

} // namespace cfd
