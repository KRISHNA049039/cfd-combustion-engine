#pragma once

#include <cmath>
#include <iostream>

namespace cfd {

/**
 * @brief 3D vector class for geometric operations
 */
class Vector3D {
public:
    double x, y, z;
    
    // Constructors
    Vector3D() : x(0.0), y(0.0), z(0.0) {}
    Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    
    // Vector operations
    Vector3D operator+(const Vector3D& v) const {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }
    
    Vector3D operator-(const Vector3D& v) const {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }
    
    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3D operator/(double scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }
    
    // Compound assignment
    Vector3D& operator+=(const Vector3D& v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    
    Vector3D& operator-=(const Vector3D& v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    
    Vector3D& operator*=(double scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    
    // Dot product
    double dot(const Vector3D& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    
    // Cross product
    Vector3D cross(const Vector3D& v) const {
        return Vector3D(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    
    // Magnitude
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    double magnitudeSquared() const {
        return x * x + y * y + z * z;
    }
    
    // Normalize
    Vector3D normalized() const {
        double mag = magnitude();
        if (mag > 1e-15) {
            return *this / mag;
        }
        return Vector3D(0, 0, 0);
    }
    
    void normalize() {
        double mag = magnitude();
        if (mag > 1e-15) {
            x /= mag; y /= mag; z /= mag;
        }
    }
    
    // Distance
    double distance(const Vector3D& v) const {
        return (*this - v).magnitude();
    }
    
    // Array access
    double& operator[](int i) {
        return (i == 0) ? x : (i == 1) ? y : z;
    }
    
    const double& operator[](int i) const {
        return (i == 0) ? x : (i == 1) ? y : z;
    }
    
    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

// Scalar multiplication (scalar * vector)
inline Vector3D operator*(double scalar, const Vector3D& v) {
    return v * scalar;
}

} // namespace cfd
