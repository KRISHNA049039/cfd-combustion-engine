#include "core/Field.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>

namespace cfd {

Field::Field(const std::string& name_, FieldType type_, int size)
    : name(name_), type(type_) {
    int components = getNumComponents();
    data.resize(size * components, 0.0);
}

Field::Field(const Field& other)
    : name(other.name), type(other.type), data(other.data) {
}

Field& Field::operator=(const Field& other) {
    if (this != &other) {
        name = other.name;
        type = other.type;
        data = other.data;
    }
    return *this;
}

double& Field::operator()(int cellId, int component) {
    int components = getNumComponents();
    return data[cellId * components + component];
}

const double& Field::operator()(int cellId, int component) const {
    int components = getNumComponents();
    return data[cellId * components + component];
}

int Field::getNumComponents() const {
    switch (type) {
        case FieldType::SCALAR: return 1;
        case FieldType::VECTOR: return 3;
        case FieldType::TENSOR: return 9;
        default: return 1;
    }
}

void Field::fill(double value) {
    std::fill(data.begin(), data.end(), value);
}

void Field::fillComponent(int component, double value) {
    int components = getNumComponents();
    int size = getSize();
    for (int i = 0; i < size; ++i) {
        data[i * components + component] = value;
    }
}

void Field::scale(double factor) {
    for (auto& val : data) {
        val *= factor;
    }
}

void Field::add(const Field& other) {
    if (data.size() != other.data.size()) {
        throw std::runtime_error("Field sizes do not match");
    }
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] += other.data[i];
    }
}

void Field::subtract(const Field& other) {
    if (data.size() != other.data.size()) {
        throw std::runtime_error("Field sizes do not match");
    }
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] -= other.data[i];
    }
}

double Field::min() const {
    if (data.empty()) return 0.0;
    return *std::min_element(data.begin(), data.end());
}

double Field::max() const {
    if (data.empty()) return 0.0;
    return *std::max_element(data.begin(), data.end());
}

double Field::mean() const {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / static_cast<double>(data.size());
}

double Field::minComponent(int component) const {
    int components = getNumComponents();
    int size = getSize();
    if (size == 0) return 0.0;
    
    double minVal = data[component];
    for (int i = 1; i < size; ++i) {
        minVal = std::min(minVal, data[i * components + component]);
    }
    return minVal;
}

double Field::maxComponent(int component) const {
    int components = getNumComponents();
    int size = getSize();
    if (size == 0) return 0.0;
    
    double maxVal = data[component];
    for (int i = 1; i < size; ++i) {
        maxVal = std::max(maxVal, data[i * components + component]);
    }
    return maxVal;
}

void Field::clamp(double minVal, double maxVal) {
    for (auto& val : data) {
        val = std::max(minVal, std::min(maxVal, val));
    }
}

void Field::clampComponent(int component, double minVal, double maxVal) {
    int components = getNumComponents();
    int size = getSize();
    for (int i = 0; i < size; ++i) {
        double& val = data[i * components + component];
        val = std::max(minVal, std::min(maxVal, val));
    }
}

bool Field::isValid() const {
    return !hasNaN() && !hasInf();
}

bool Field::hasNaN() const {
    for (const auto& val : data) {
        if (std::isnan(val)) return true;
    }
    return false;
}

bool Field::hasInf() const {
    for (const auto& val : data) {
        if (std::isinf(val)) return true;
    }
    return false;
}

} // namespace cfd
