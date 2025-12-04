#include <gtest/gtest.h>
#include "core/Vector3D.h"

using namespace cfd;

TEST(Vector3DTest, Construction) {
    Vector3D v1;
    EXPECT_DOUBLE_EQ(v1.x, 0.0);
    EXPECT_DOUBLE_EQ(v1.y, 0.0);
    EXPECT_DOUBLE_EQ(v1.z, 0.0);
    
    Vector3D v2(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v2.x, 1.0);
    EXPECT_DOUBLE_EQ(v2.y, 2.0);
    EXPECT_DOUBLE_EQ(v2.z, 3.0);
}

TEST(Vector3DTest, Addition) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(4.0, 5.0, 6.0);
    Vector3D v3 = v1 + v2;
    
    EXPECT_DOUBLE_EQ(v3.x, 5.0);
    EXPECT_DOUBLE_EQ(v3.y, 7.0);
    EXPECT_DOUBLE_EQ(v3.z, 9.0);
}

TEST(Vector3DTest, DotProduct) {
    Vector3D v1(1.0, 0.0, 0.0);
    Vector3D v2(0.0, 1.0, 0.0);
    
    EXPECT_DOUBLE_EQ(v1.dot(v2), 0.0);
    EXPECT_DOUBLE_EQ(v1.dot(v1), 1.0);
}

TEST(Vector3DTest, CrossProduct) {
    Vector3D v1(1.0, 0.0, 0.0);
    Vector3D v2(0.0, 1.0, 0.0);
    Vector3D v3 = v1.cross(v2);
    
    EXPECT_DOUBLE_EQ(v3.x, 0.0);
    EXPECT_DOUBLE_EQ(v3.y, 0.0);
    EXPECT_DOUBLE_EQ(v3.z, 1.0);
}

TEST(Vector3DTest, Magnitude) {
    Vector3D v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.magnitude(), 5.0);
}

TEST(Vector3DTest, Normalization) {
    Vector3D v(3.0, 4.0, 0.0);
    Vector3D vn = v.normalized();
    
    EXPECT_NEAR(vn.magnitude(), 1.0, 1e-10);
    EXPECT_DOUBLE_EQ(vn.x, 0.6);
    EXPECT_DOUBLE_EQ(vn.y, 0.8);
}
