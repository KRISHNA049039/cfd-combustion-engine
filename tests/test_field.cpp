#include <gtest/gtest.h>
#include "core/Field.h"
#include "core/FieldManager.h"
#include <cmath>

using namespace cfd;

TEST(FieldTest, ScalarField) {
    Field field("temperature", FieldType::SCALAR, 10);
    
    EXPECT_EQ(field.name, "temperature");
    EXPECT_EQ(field.getNumComponents(), 1);
    EXPECT_EQ(field.getSize(), 10);
    
    field(5) = 300.0;
    EXPECT_DOUBLE_EQ(field(5), 300.0);
}

TEST(FieldTest, VectorField) {
    Field field("velocity", FieldType::VECTOR, 10);
    
    EXPECT_EQ(field.getNumComponents(), 3);
    EXPECT_EQ(field.getSize(), 10);
    
    field(3, 0) = 1.0;
    field(3, 1) = 2.0;
    field(3, 2) = 3.0;
    
    EXPECT_DOUBLE_EQ(field(3, 0), 1.0);
    EXPECT_DOUBLE_EQ(field(3, 1), 2.0);
    EXPECT_DOUBLE_EQ(field(3, 2), 3.0);
}

TEST(FieldTest, TensorField) {
    Field field("stress", FieldType::TENSOR, 5);
    
    EXPECT_EQ(field.getNumComponents(), 9);
    EXPECT_EQ(field.getSize(), 5);
}

TEST(FieldTest, FillOperation) {
    Field field("temperature", FieldType::SCALAR, 10);
    field.fill(300.0);
    
    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(field(i), 300.0);
    }
}

TEST(FieldTest, ScaleOperation) {
    Field field("pressure", FieldType::SCALAR, 5);
    field.fill(100.0);
    field.scale(2.0);
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_DOUBLE_EQ(field(i), 200.0);
    }
}

TEST(FieldTest, AddOperation) {
    Field field1("temp1", FieldType::SCALAR, 5);
    Field field2("temp2", FieldType::SCALAR, 5);
    
    field1.fill(100.0);
    field2.fill(50.0);
    field1.add(field2);
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_DOUBLE_EQ(field1(i), 150.0);
    }
}

TEST(FieldTest, Statistics) {
    Field field("data", FieldType::SCALAR, 5);
    field(0) = 1.0;
    field(1) = 2.0;
    field(2) = 3.0;
    field(3) = 4.0;
    field(4) = 5.0;
    
    EXPECT_DOUBLE_EQ(field.min(), 1.0);
    EXPECT_DOUBLE_EQ(field.max(), 5.0);
    EXPECT_DOUBLE_EQ(field.mean(), 3.0);
}

TEST(FieldTest, Clamping) {
    Field field("temperature", FieldType::SCALAR, 5);
    field(0) = 100.0;
    field(1) = 200.0;
    field(2) = 300.0;
    field(3) = 400.0;
    field(4) = 500.0;
    
    field.clamp(150.0, 350.0);
    
    EXPECT_DOUBLE_EQ(field(0), 150.0);
    EXPECT_DOUBLE_EQ(field(1), 200.0);
    EXPECT_DOUBLE_EQ(field(2), 300.0);
    EXPECT_DOUBLE_EQ(field(3), 350.0);
    EXPECT_DOUBLE_EQ(field(4), 350.0);
}

TEST(FieldTest, Validation) {
    Field field("data", FieldType::SCALAR, 3);
    field.fill(1.0);
    
    EXPECT_TRUE(field.isValid());
    EXPECT_FALSE(field.hasNaN());
    EXPECT_FALSE(field.hasInf());
    
    field(1) = std::nan("");
    EXPECT_FALSE(field.isValid());
    EXPECT_TRUE(field.hasNaN());
    
    field(1) = 1.0;
    field(2) = std::numeric_limits<double>::infinity();
    EXPECT_FALSE(field.isValid());
    EXPECT_TRUE(field.hasInf());
}

TEST(FieldTest, CopyConstructor) {
    Field field1("original", FieldType::SCALAR, 5);
    field1.fill(42.0);
    
    Field field2(field1);
    
    EXPECT_EQ(field2.name, "original");
    EXPECT_EQ(field2.getSize(), 5);
    EXPECT_DOUBLE_EQ(field2(0), 42.0);
}

TEST(FieldManagerTest, RegisterAndRetrieve) {
    FieldManager manager;
    
    manager.registerField("pressure", FieldType::SCALAR, 100);
    manager.registerField("velocity", FieldType::VECTOR, 100);
    
    EXPECT_TRUE(manager.hasField("pressure"));
    EXPECT_TRUE(manager.hasField("velocity"));
    EXPECT_FALSE(manager.hasField("temperature"));
    
    Field& p = manager.getField("pressure");
    EXPECT_EQ(p.name, "pressure");
}

TEST(FieldManagerTest, FieldQueries) {
    FieldManager manager;
    
    manager.registerField("pressure", FieldType::SCALAR, 100);
    manager.registerField("temperature", FieldType::SCALAR, 100);
    manager.registerField("velocity", FieldType::VECTOR, 100);
    
    EXPECT_EQ(manager.getNumFields(), 3);
    
    auto scalarFields = manager.getScalarFieldNames();
    auto vectorFields = manager.getVectorFieldNames();
    
    EXPECT_EQ(scalarFields.size(), 2);
    EXPECT_EQ(vectorFields.size(), 1);
}

TEST(FieldManagerTest, BulkOperations) {
    FieldManager manager;
    
    manager.registerField("field1", FieldType::SCALAR, 10);
    manager.registerField("field2", FieldType::SCALAR, 10);
    
    manager.fillAll(100.0);
    
    EXPECT_DOUBLE_EQ(manager.getField("field1")(0), 100.0);
    EXPECT_DOUBLE_EQ(manager.getField("field2")(0), 100.0);
    
    manager.scaleAll(2.0);
    
    EXPECT_DOUBLE_EQ(manager.getField("field1")(0), 200.0);
    EXPECT_DOUBLE_EQ(manager.getField("field2")(0), 200.0);
}

TEST(FieldManagerTest, Validation) {
    FieldManager manager;
    
    manager.registerField("field1", FieldType::SCALAR, 5);
    manager.registerField("field2", FieldType::SCALAR, 5);
    
    manager.fillAll(1.0);
    EXPECT_TRUE(manager.validateAll());
    
    manager.getField("field1")(2) = std::nan("");
    EXPECT_FALSE(manager.validateAll());
    
    auto invalid = manager.getInvalidFields();
    EXPECT_EQ(invalid.size(), 1);
    EXPECT_EQ(invalid[0], "field1");
}

TEST(FieldManagerTest, MemoryUsage) {
    FieldManager manager;
    
    manager.registerField("scalar", FieldType::SCALAR, 100);
    manager.registerField("vector", FieldType::VECTOR, 100);
    
    size_t expected = (100 * 1 + 100 * 3) * sizeof(double);
    EXPECT_EQ(manager.getTotalMemoryUsage(), expected);
}

TEST(FieldManagerTest, Resize) {
    FieldManager manager;
    
    manager.registerField("field1", FieldType::SCALAR, 10);
    EXPECT_EQ(manager.getField("field1").getSize(), 10);
    
    manager.resize(20);
    EXPECT_EQ(manager.getField("field1").getSize(), 20);
}

TEST(FieldManagerTest, RemoveField) {
    FieldManager manager;
    
    manager.registerField("temp", FieldType::SCALAR, 10);
    EXPECT_TRUE(manager.hasField("temp"));
    
    manager.removeField("temp");
    EXPECT_FALSE(manager.hasField("temp"));
}
