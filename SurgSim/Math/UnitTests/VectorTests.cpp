// This file is a part of the OpenSurgSim project.
// Copyright 2012-2013, SimQuest Solutions Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file
/// Tests that exercise the functionality of our vector typedefs, which come
/// straight from Eigen.

#include <vector>

#include <math.h>
#include "SurgSim/Math/Vector.h"
#include "SurgSim/Math/MathConvert.h"
#include "gtest/gtest.h"

// Define test fixture class templates.
// We don't really need fixtures as such, but the templatization encodes type.

template <class T>
class VectorTestBase : public testing::Test
{
public:
	typedef T Scalar;
};



template <class T>
class Vector2Tests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector2;
};

// This used to contain aligned (via Eigen::AutoAlign) vector type aliases, but we got rid of those.
typedef ::testing::Types<SurgSim::Math::Vector2d,
						 SurgSim::Math::Vector2f> Vector2Variants;
TYPED_TEST_CASE(Vector2Tests, Vector2Variants);


template <class T>
class Vector3Tests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector3;
};

// This used to contain aligned (via Eigen::AutoAlign) vector type aliases, but we got rid of those.
typedef ::testing::Types<SurgSim::Math::Vector3d,
						 SurgSim::Math::Vector3f> Vector3Variants;
TYPED_TEST_CASE(Vector3Tests, Vector3Variants);


template <class T>
class Vector4Tests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector4;
};

// This used to contain aligned (via Eigen::AutoAlign) vector type aliases, but we got rid of those.
typedef ::testing::Types<SurgSim::Math::Vector4d,
						 SurgSim::Math::Vector4f> Vector4Variants;
TYPED_TEST_CASE(Vector4Tests, Vector4Variants);



template <class T>
class AllVectorTests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector;
};

template <class T>
class AllDynamicVectorTests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector;
};

// This used to contain aligned (via Eigen::AutoAlign) vector type aliases, but we got rid of those.
typedef ::testing::Types<SurgSim::Math::Vector2d,
						 SurgSim::Math::Vector2f,
						 SurgSim::Math::Vector3d,
						 SurgSim::Math::Vector3f,
						 SurgSim::Math::Vector4d,
						 SurgSim::Math::Vector4f> AllVectorVariants;
TYPED_TEST_CASE(AllVectorTests, AllVectorVariants);

typedef ::testing::Types<Eigen::VectorXd,
						 Eigen::VectorXf,
						 SurgSim::Math::Vector> AllDynamicVectorVariants;
TYPED_TEST_CASE(AllDynamicVectorTests, AllDynamicVectorVariants);

template <class T>
class UnalignedVectorTests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector;
};

template <class T>
class UnalignedDynamicVectorTests : public VectorTestBase<typename T::Scalar>
{
public:
	typedef T Vector;
};

typedef ::testing::Types<SurgSim::Math::Vector2d,
						 SurgSim::Math::Vector2f,
						 SurgSim::Math::Vector3d,
						 SurgSim::Math::Vector3f,
						 SurgSim::Math::Vector4d,
						 SurgSim::Math::Vector4f> UnalignedVectorVariants;
TYPED_TEST_CASE(UnalignedVectorTests, UnalignedVectorVariants);

typedef ::testing::Types<Eigen::VectorXd,
						 Eigen::VectorXf,
						 SurgSim::Math::Vector> UnalignedDynamicVectorVariants;
TYPED_TEST_CASE(UnalignedDynamicVectorTests, UnalignedDynamicVectorVariants);


// Now we're ready to start testing...


// ==================== CONSTRUCTION & INITIALIZATION ====================

/// Test that vectors can be constructed.
TYPED_TEST(Vector2Tests, CanConstruct)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	// Warning: Eigen *does not* provide a 1-argument constructor that
	// initializes all elements to the same value!  If you do something like
	//    SurgSim::Math::Vector2f oneArg2f(1.23f);
	// the argument is converted to an integral type, interpreted as a size,
	// and promptly ignored because the size is fixed.  Oops.
	// To generate a constant vector, use Vector2f::Constant(val).

	Vector2 default2;
	Vector2 twoArg2(static_cast<T>(1.0), static_cast<T>(2.0));
}

/// Test that vectors can be constructed.
TYPED_TEST(Vector3Tests, CanConstruct)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename TestFixture::Scalar  T;

	// Warning: Eigen *does not* provide a 1-argument constructor that
	// initializes all elements to the same value!  If you do something like
	//    SurgSim::Math::Vector2fu oneArg2fu(1.23f);
	// the argument is converted to an integral type, interpreted as a size,
	// and promptly ignored because the size is fixed.  Oops.
	// To generate a constant vector, use Vector2f::Constant(val).

	Vector3 default3;
	Vector3 threeArg3(static_cast<T>(1.0), static_cast<T>(2.0), static_cast<T>(3.0));
}

/// Test that vectors can be constructed.
TYPED_TEST(Vector4Tests, CanConstruct)
{
	typedef typename TestFixture::Vector4 Vector4;
	typedef typename TestFixture::Scalar  T;

	// Warning: Eigen *does not* provide a 1-argument constructor that
	// initializes all elements to the same value!  If you do something like
	//    SurgSim::Math::Vector2fu oneArg2fu(1.23f);
	// the argument is converted to an integral type, interpreted as a size,
	// and promptly ignored because the size is fixed.  Oops.
	// To generate a constant vector, use Vector2f::Constant(val).

	Vector4 default4;
	Vector4 fourArg4(static_cast<T>(1.0), static_cast<T>(2.0), static_cast<T>(3.0), static_cast<T>(4.0));
}

/// Test that the N-argument constructor properly initializes vectors.
TYPED_TEST(Vector2Tests, NArgumentConstructorInitialization)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	Vector2 vector(static_cast<T>(1.01), static_cast<T>(1.02));
	EXPECT_NEAR(1.01, vector[0], 1e-6) << "X wasn't properly initialized.";
	EXPECT_NEAR(1.02, vector[1], 1e-6) << "Y wasn't properly initialized.";
}

/// Test that the N-argument constructor properly initializes vectors.
TYPED_TEST(Vector3Tests, NArgumentConstructorInitialization)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename TestFixture::Scalar  T;

	Vector3 vector(static_cast<T>(1.03), static_cast<T>(1.04), static_cast<T>(1.05));
	EXPECT_NEAR(1.03, vector[0], 1e-6) << "X wasn't properly initialized.";
	EXPECT_NEAR(1.04, vector[1], 1e-6) << "Y wasn't properly initialized.";
	EXPECT_NEAR(1.05, vector[2], 1e-6) << "Z wasn't properly initialized.";
}

/// Test that the N-argument constructor properly initializes vectors.
TYPED_TEST(Vector4Tests, NArgumentConstructorInitialization)
{
	typedef typename TestFixture::Vector4 Vector4;
	typedef typename TestFixture::Scalar  T;

	Vector4 vector(static_cast<T>(1.06), static_cast<T>(1.07), static_cast<T>(1.08), static_cast<T>(1.09));
	EXPECT_NEAR(1.06, vector[0], 1e-6) << "X wasn't properly initialized.";
	EXPECT_NEAR(1.07, vector[1], 1e-6) << "Y wasn't properly initialized.";
	EXPECT_NEAR(1.08, vector[2], 1e-6) << "Z wasn't properly initialized.";
	EXPECT_NEAR(1.09, vector[3], 1e-6) << "W wasn't properly initialized.";
}

/// Test that the default constructor DOESN'T initialize vectors.
//
// Only test the non-vectorized versions.  Otherwise, we'd need to
// allocate memory in a way that guarantees Eigen-compatible alignment.
//
// TODO(bert): There is some Eigen flag that causes matrices and vectors to be
//   initialized after all!  We should check for that here.
TYPED_TEST(UnalignedVectorTests, DefaultConstructorInitialization)
{
	typedef typename TestFixture::Vector Vector;
	const int SIZE = Vector::RowsAtCompileTime;

	EXPECT_TRUE(SIZE >= 2 && SIZE <= 4);
	EXPECT_EQ(1, Vector::ColsAtCompileTime);

	// Allocate a buffer for various vector types on stack, based on the size
	// of the largest object we're testing.  Objects will be allocated inside
	// the buffer using the placement syntax for the new() operator.
	unsigned char buffer[sizeof(Vector)];

	{
		// Please don't write production (non-test) code that looks like this. =)
		memset(&buffer, 0xF0, sizeof(buffer));
		Vector* vector = new(&buffer) Vector;
		for (int i = 0;  i < SIZE; ++i)
		{
			EXPECT_NE(0.0f, (*vector)[i]) << i << " was NOT supposed to be zeroed.";
		}
		// Destroying the object is a good idea, even if unnecessary here:
		vector->Vector::~Vector();
	}
}

/// Test setting the vector using the << syntax.
TYPED_TEST(Vector2Tests, ShiftCommaInitialization)
{
	typedef typename TestFixture::Vector2 Vector2;

	Vector2 vector;
	// Initialize elements in order.  Do NOT put parentheses around the list!
	vector << 1.1f, 1.2f;
	EXPECT_NEAR(2.3, vector.sum(), 1e-6) << "initialization was incorrect: " << vector;
}

/// Test setting the vector using the << syntax.
TYPED_TEST(Vector3Tests, ShiftCommaInitialization)
{
	typedef typename TestFixture::Vector3 Vector3;

	Vector3 vector;
	// Initialize elements in order.  Do NOT put parentheses around the list!
	vector << 1.1f, 1.2f, 1.3f;
	EXPECT_NEAR(3.6, vector.sum(), 1e-6) << "initialization was incorrect: " << vector;
}

/// Test setting the vector using the << syntax.
TYPED_TEST(Vector4Tests, ShiftCommaInitialization)
{
	typedef typename TestFixture::Vector4 Vector4;

	Vector4 vector;
	// Initialize elements in order.  Do NOT put parentheses around the list!
	vector << 1.1f, 1.2f, 1.3f, 1.4f;
	EXPECT_NEAR(5.0, vector.sum(), 1e-6) << "initialization was incorrect: " << vector;
}

/// Test getting a zero value usable in expressions.
TYPED_TEST(AllVectorTests, ZeroValue)
{
	typedef typename TestFixture::Vector Vector;
	const int SIZE = Vector::RowsAtCompileTime;

	Vector vector = 1000 * Vector::Zero();
	for (int i = 0;  i < SIZE; ++i)
	{
		EXPECT_NEAR(0.0, vector[i], 1e-20) << i << " wasn't properly initialized.";
	}
}

/// Test setting vectors to 0.
TYPED_TEST(AllVectorTests, SetToZero)
{
	typedef typename TestFixture::Vector Vector;
	const int SIZE = Vector::RowsAtCompileTime;

	Vector vector;
	vector.setZero();
	for (int i = 0;  i < SIZE; ++i)
	{
		EXPECT_NEAR(0.0, vector[i], 1e-20) << i << " wasn't properly cleared.";
	}
}

/// Test getting a constant value usable in expressions.
TYPED_TEST(AllVectorTests, ConstantValue)
{
	typedef typename TestFixture::Vector Vector;
	const int SIZE = Vector::RowsAtCompileTime;

	Vector vector = 2 * Vector::Constant(0.5f);
	for (int i = 0;  i < SIZE; ++i)
	{
		EXPECT_NEAR(1.0, vector[i], 1e-6) << i << " wasn't properly initialized.";
	}
}

/// Test setting vectors to a constant.
TYPED_TEST(AllVectorTests, SetToConstant)
{
	typedef typename TestFixture::Vector Vector;
	const int SIZE = Vector::RowsAtCompileTime;

	Vector vector;
	vector.setConstant(7.2f);
	for (int i = 0;  i < SIZE; ++i)
	{
		EXPECT_NEAR(7.2, vector[i], 1e-6) << i << " wasn't properly initialized.";
	}
}

/// Test initializing from a float array.
TYPED_TEST(AllVectorTests, SetFromArray)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	// This array has more elements than we will need.
	// The element type must match the vector!
	const T inputArray[5]  = { 0.1f, 1.2f, 2.3f, 3.4f, 4.5f };

	Vector vector(inputArray);
	for (int i = 0;  i < SIZE; ++i)
	{
		EXPECT_NEAR(0.1 + i*1.1, vector[i], 1e-6) << i << " wasn't properly initialized.";
	}
}

// Test conversion to and from yaml node
TYPED_TEST(AllVectorTests, YamlConvert)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;

	T testData[5] = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	Vector original(testData);

	YAML::Node node;

	ASSERT_NO_THROW(node = original);

	EXPECT_TRUE(node.IsSequence());
	EXPECT_EQ(original.size(), node.size());

	Vector expected;

	ASSERT_NO_THROW(expected = node.as<Vector>());
	EXPECT_TRUE(original.isApprox(expected));
}


/// Test assignment.
TYPED_TEST(AllVectorTests, Assign)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArrayA[5]  = { 6.1f, 6.2f, 6.3f, 6.4f, 6.5f };
	const T inputArrayB[5]  = { 7.1f, 7.2f, 7.3f, 7.4f, 7.5f };

	Vector a(inputArrayA);
	// sum of the first SIZE elements of inputArrayA
	T expectedSumA = SIZE * (SIZE*0.05f + 6.05f);
	EXPECT_NEAR(expectedSumA, a.sum(), 1e-6);
	const Vector b(inputArrayB);
	// sum of the first SIZE elements of inputArrayB
	T expectedSumB = SIZE * (SIZE*0.05f + 7.05f);
	EXPECT_NEAR(expectedSumB, b.sum(), 1e-6);
	a = b;
	EXPECT_NEAR(expectedSumB, a.sum(), 1e-6);
}

// ==================== ARITHMETIC ====================

/// Negation (unary minus).
TYPED_TEST(AllVectorTests, Negate)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector n = -v;
	EXPECT_NEAR(-expectedSum, n.sum(), 1e-6);
}

/// Addition.
TYPED_TEST(AllVectorTests, Add)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector w = v + Vector::Ones() + v;

	EXPECT_NEAR(2*expectedSum + SIZE, w.sum(), 1e-6);
}

/// Subtraction.
TYPED_TEST(AllVectorTests, Subtract)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector w = v - Vector::Ones();

	EXPECT_NEAR(expectedSum - SIZE, w.sum(), 1e-6);
}

/// Incrementing by a value.
TYPED_TEST(AllVectorTests, AddTo)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	v += Vector::Ones();
	EXPECT_NEAR(expectedSum + SIZE, v.sum(), 1e-6);
}

/// Decrementing by a value.
TYPED_TEST(AllVectorTests, SubtractFrom)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	v -= Vector::Ones();
	EXPECT_NEAR(expectedSum - SIZE, v.sum(), 1e-6);
}

/// Vector-scalar multiplication.
TYPED_TEST(AllVectorTests, MultiplyVectorScalar)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector w = v * 1.23f;

	EXPECT_NEAR(1.23*expectedSum, w.sum(), 1e-6);
}

/// Scalar-vector multiplication.
TYPED_TEST(AllVectorTests, MultiplyScalarVector)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector w = 1.23f * v;

	EXPECT_NEAR(1.23*expectedSum, w.sum(), 1e-6);
}

/// Division by scalar.
TYPED_TEST(AllVectorTests, DivideScalar)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector w = v / 1.23f;

	EXPECT_NEAR(expectedSum / 1.23, w.sum(), 1e-6);
}

/// Component-wise multiplication.
TYPED_TEST(AllVectorTests, ComponentwiseMultiply)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	// sum of the squares of the first SIZE elements of inputArray
	double expectedSumSquares = SIZE * (SIZE * (SIZE*0.03 + 0.885) + 8.695);

	Vector v(inputArray);
	// use the component-wise Eigen matrix operation:
	Vector w = v.cwiseProduct(v);
	EXPECT_NEAR(expectedSumSquares, w.sum(), 1e-6);
	// OR, the same thing done via conversion to arrays:
	w = v.array() * v.array();
	EXPECT_NEAR(expectedSumSquares, w.sum(), 1e-6);
}

/// Component-wise division.
TYPED_TEST(AllVectorTests, ComponentwiseDivide)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };

	Vector v(inputArray);
	Vector u = 2*v;
	// use the component-wise Eigen matrix operation:
	Vector w = u.cwiseQuotient(v);
	EXPECT_NEAR(2*SIZE, w.sum(), 1e-6);
	// OR, the same thing done via conversion to arrays:
	w = u.array() / v.array();
	EXPECT_NEAR(2*SIZE, w.sum(), 1e-6);
}

/// Dot product.
TYPED_TEST(AllVectorTests, DotProduct)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	// sum of the squares of the first SIZE elements of inputArray
	double expectedSumSquares = SIZE * (SIZE * (SIZE*0.03 + 0.885) + 8.695);

	Vector v(inputArray);
	EXPECT_NEAR(expectedSumSquares, v.dot(v), 1e-6);
}

/// Cross product.
TYPED_TEST(Vector3Tests, CrossProduct)
{
	typedef typename TestFixture::Vector3 Vector3;

	Vector3 v;
	v << 3.4f, 5.6f, 7.8f;
	Vector3 u = -v;
	u[0] = v[0];
	Vector3 w = v.cross(u);
	std::cout << w << std::endl;
	EXPECT_NEAR(0, w[0], 1e-6);
	EXPECT_GT(abs(w[1]), 1);  // nonzero
	EXPECT_GT(abs(w[2]), 1);  // nonzero
}

/// Outer product.
TYPED_TEST(AllVectorTests, OuterProduct)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;
	typedef Eigen::Matrix<T, SIZE, SIZE> Matrix;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	// sum of the squares of the first SIZE elements of inputArray
	double expectedSumSquares = SIZE * (SIZE * (SIZE*0.03 + 0.885) + 8.695);

	Vector v(inputArray);
	// You have to write out the outer product, like this:
	Matrix m = v * v.transpose();

	// TODO(bert): maybe needs better testing here?
	Vector u = v / v.squaredNorm();
	EXPECT_NEAR(expectedSumSquares, (m*u).squaredNorm(), 1e-3);
}

/// Euclidean norm and its square.
TYPED_TEST(AllVectorTests, NormAndSquared)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	// sum of the squares of the first SIZE elements of inputArray
	double expectedSumSquares = SIZE * (SIZE * (SIZE*0.03 + 0.885) + 8.695);

	Vector v(inputArray);
	EXPECT_NEAR(expectedSumSquares, v.squaredNorm(), 1e-6);
	EXPECT_NEAR(sqrt(expectedSumSquares), v.norm(), 1e-6);
}

/// L1 (Manhattan) norm and L_Infinity (largest absolute value) norm.
TYPED_TEST(AllVectorTests, L1NormAndLInfNorm)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 2.1f, 2.2f, 2.3f, 2.4f, 2.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 2.05);

	Vector v(inputArray);
	Vector w = -v;
	// Ugh, "template" is required to get this to parse properly.  This is
	// triggered because the test is a part of a template class; you don't
	// need to do this in a non-template context.
	EXPECT_NEAR(expectedSum, v.template lpNorm<1>(), 1e-6);
	EXPECT_NEAR(expectedSum, w.template lpNorm<1>(), 1e-6);
	EXPECT_NEAR(inputArray[SIZE-1], v.template lpNorm<Eigen::Infinity>(), 1e-6);
	EXPECT_NEAR(inputArray[SIZE-1], w.template lpNorm<Eigen::Infinity>(), 1e-6);
}

/// Normalization of vectors.
TYPED_TEST(AllVectorTests, Normalize)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	// sum of the squares of the first SIZE elements of inputArray
	double expectedSumSquares = SIZE * (SIZE * (SIZE*0.03 + 0.885) + 8.695);

	Vector v(inputArray);
	EXPECT_NEAR(sqrt(expectedSumSquares), v.norm(), 1e-6);

	// normalized() RETURNS the normalized vector, leaving original unchanged.
	Vector u = v.normalized();
	EXPECT_NEAR(1, u.norm(), 1e-6);
	EXPECT_NEAR(sqrt(expectedSumSquares), v.norm(), 1e-6);
	// normalize() NORMALIZES the vector, modifying it.
	v.normalize();
	EXPECT_NEAR(1, v.norm(), 1e-6);
	EXPECT_NEAR(0, (u - v).norm(), 1e-6);
}

/// Minimum and maximum elements.
TYPED_TEST(AllVectorTests, MinAndMax)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };

	Vector v(inputArray);
	EXPECT_NEAR(inputArray[0], v.minCoeff(), 1e-6);
	EXPECT_NEAR(inputArray[SIZE-1], v.maxCoeff(), 1e-6);
}

// ==================== SUBVECTORS (EXTENDING/SHRINKING) ====================

/// Extending vectors using the head<r>() syntax.
TYPED_TEST(Vector2Tests, Extend2to3)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector2 vector2;
	vector2 << 1.1f, 1.2f;
	Vector3 vector3;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector3.template head<2>() = vector2;
	vector3[2] = 0;
	EXPECT_NEAR(2.3, vector3.sum(), 1e-6) << "extending was incorrect: " << vector3;
}

/// Extending vectors using the head(r) syntax.
TYPED_TEST(Vector2Tests, DynamicExtend2to3)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector2 vector2;
	vector2 << 1.1f, 1.2f;
	Vector3 vector3;
	vector3.head(2) = vector2;
	vector3[2] = 0;
	EXPECT_NEAR(2.3, vector3.sum(), 1e-6) << "extending was incorrect: " << vector3;
}

/// Extending vectors using the block<r,c>() syntax.
TYPED_TEST(Vector2Tests, BlockExtend2to3)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector2 vector2;
	vector2 << 1.1f, 1.2f;
	Vector3 vector3;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector3.template block<2, 1>(0, 0) = vector2;
	vector3(2, 0) = 0;
	EXPECT_NEAR(2.3, vector3.sum(), 1e-6) << "extending was incorrect: " << vector3;
}

/// Extending vectors using the block(i,j,r,c) syntax.
TYPED_TEST(Vector2Tests, DynamicBlockExtend2to3)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector2 vector2;
	vector2 << 1.1f, 1.2f;
	Vector3 vector3;
	vector3.block(0, 0, 2, 1) = vector2;
	vector3(2, 0) = 0;
	EXPECT_NEAR(2.3, vector3.sum(), 1e-6) << "extending was incorrect: " << vector3;
}

/// Shrinking vectors using the head<r>() syntax.
TYPED_TEST(Vector2Tests, Shrink3to2)
{
	typedef typename TestFixture::Vector2 Vector2;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector3 vector3;
	vector3 << 1.1f, 1.2f, 1.3f;
	Vector2 vector2;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector2 = vector3.template head<2>();
	EXPECT_NEAR(2.3, vector2.sum(), 1e-6) << "shrinking was incorrect: " << vector2;
}

/// Extending vectors using the head<r>() syntax.
TYPED_TEST(Vector3Tests, Extend2to3)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 2, 1> Vector2;

	Vector2 vector2;
	vector2 << 1.1f, 1.2f;
	Vector3 vector3;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector3.template head<2>() = vector2;
	vector3[2] = 0;
	EXPECT_NEAR(2.3, vector3.sum(), 1e-6) << "extending was incorrect: " << vector3;
}

/// Shrinking vectors using the head<r>() syntax.
TYPED_TEST(Vector3Tests, Shrink3to2)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 2, 1> Vector2;

	Vector3 vector3;
	vector3 << 1.1f, 1.2f, 1.3f;
	Vector2 vector2;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector2 = vector3.template head<2>();
	EXPECT_NEAR(2.3, vector2.sum(), 1e-6) << "shrinking was incorrect" << vector2;
}

/// Extending vectors using the head<r>() syntax.
TYPED_TEST(Vector3Tests, Extend3to4)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 4, 1> Vector4;

	Vector3 vector3;
	vector3 << 1.1f, 1.2f, 1.3f;
	Vector4 vector4;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector4.template head<3>() = vector3;
	vector4[3] = 0;
	EXPECT_NEAR(3.6, vector4.sum(), 1e-6) << "extending was incorrect" << vector4;
}

/// Shrinking vectors using the head<r>() syntax.
TYPED_TEST(Vector3Tests, Shrink4to3)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 4, 1> Vector4;

	Vector4 vector4;
	vector4 << 1.1f, 1.2f, 1.3f, 1.4f;
	Vector3 vector3;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector3 = vector4.template head<3>();
	EXPECT_NEAR(3.6, vector3.sum(), 1e-6) << "shrinking was incorrect" << vector3;
}

/// Extending vectors using the head<r>() syntax.
TYPED_TEST(Vector4Tests, Extend3to4)
{
	typedef typename TestFixture::Vector4 Vector4;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector3 vector3;
	vector3 << 1.1f, 1.2f, 1.3f;
	Vector4 vector4;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector4.template head<3>() = vector3;
	vector4[3] = 0;
	EXPECT_NEAR(3.6, vector4.sum(), 1e-6) << "extending was incorrect" << vector4;
}

/// Shrinking vectors using the head<r>() syntax.
TYPED_TEST(Vector4Tests, Shrink4to3)
{
	typedef typename TestFixture::Vector4 Vector4;
	typedef typename TestFixture::Scalar  T;

	typedef Eigen::Matrix<T, 3, 1> Vector3;

	Vector4 vector4;
	vector4 << 1.1f, 1.2f, 1.3f, 1.4f;
	Vector3 vector3;
	// Ugh, this is efficient but "template" is required to get it to parse
	// properly.  This is triggered because the test is a part of a template
	// class; you don't need to do this in a non-template context.
	vector3 = vector4.template head<3>();
	EXPECT_NEAR(3.6, vector3.sum(), 1e-6) << "shrinking was incorrect" << vector3;
}

/// Extend Euclidean N-vector [a_i] to homogeneous (N+1)-vector [a_i 1].
TYPED_TEST(AllVectorTests, HomogeneousExtend)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;
	typedef Eigen::Matrix<T, SIZE+1, 1> HVector;

	const T inputArray[5]  = { 10.1f, 10.2f, 10.3f, 10.4f, 10.5f };

	Vector v(inputArray);
	HVector h = v.homogeneous();
	for (int i = 0;  i < SIZE;  ++i)
	{
		EXPECT_NEAR(inputArray[i], h[i], 1e-6) << "homogeneous form garbled.";
	}
	EXPECT_NEAR(1.0, h[SIZE], 1e-6) << "homogeneous form garbled.";
}

/// Shrink a homogeneous (N+1)-vector [a_i 1] to Euclidean N-vector [a_i].
///
///  Note that if the last element is not 1, the result will be divided by it,
///  which is helpful in projective geometry but may not be what you wanted!
TYPED_TEST(AllVectorTests, HomogeneousShrink)
{
	typedef typename TestFixture::Vector HVector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = HVector::RowsAtCompileTime - 1;
	typedef Eigen::Matrix<T, SIZE, 1> Vector;

	const T inputArray[5]  = { 10.1f, 10.2f, 10.3f, 10.4f, 10.5f };

	HVector h(inputArray);
	h[SIZE] = 2;  // makes calculating expected values simpler =)
	Vector v = h.hnormalized();
	for (int i = 0;  i < SIZE;  ++i)
	{
		EXPECT_NEAR(inputArray[i]/2, v[i], 1e-6) << "Euclidean form from homogeneous garbled.";
	}
}

// ==================== TYPE CONVERSION ====================

/// Typecasting vectors (double <-> float conversions).
TYPED_TEST(AllVectorTests, TypeCasting)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;
	typedef Eigen::Matrix<double, SIZE, 1> Vectord;
	typedef Eigen::Matrix<float,  SIZE, 1> Vectorf;

	const T inputArray[5]  = { 12.1f, 12.2f, 12.3f, 12.4f, 12.5f };
	// sum of the first SIZE elements of inputArray
	double expectedSum = SIZE * (SIZE*0.05 + 12.05);

	Vector v(inputArray);
	// Ugh, "template" is required to get this to parse properly.  This is
	// triggered because the test is a part of a template class; you don't
	// need to do this in a non-template context.
	Vectord vd = v.template cast<double>();
	EXPECT_NEAR(expectedSum, vd.sum(), 1e-6);
	Vectorf vf = v.template cast<float>();
	EXPECT_NEAR(expectedSum, vf.sum(), 1e-4);
}

// ==================== MISCELLANEOUS ====================

/// Reading from and writing to arrays or blocks of double/float in memory.
TYPED_TEST(AllVectorTests, ArrayReadWrite)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;
	const int SIZE = Vector::RowsAtCompileTime;

	const T inputArray[5]  = { 12.1f, 12.2f, 12.3f, 12.4f, 12.5f };
	T outputArray[5];

	Eigen::Map<const Vector> v_in(inputArray);
	Vector v1 = v_in;

	Eigen::Map<Vector> v_out(outputArray);
	v_out = v1;

	for (int i = 0;  i < SIZE;  ++i)
	{
		EXPECT_NEAR(inputArray[i], outputArray[i], 1e-6);
	}
}

TYPED_TEST(AllVectorTests, Interpolate)
{
	typedef typename TestFixture::Vector Vector;
	typedef typename TestFixture::Scalar T;

	T epsilon = static_cast<T>(1e-6);

	T prevArray[5] = { 3.1f, 3.4f, 3.7f, 4.0f, 4.3f };
	T nextArray[5] = { 7.2f, 0.6f, 4.8f, 5.1f, 8.9f };
	T interpArray[5];

	Vector prev(prevArray);
	Vector next(nextArray);
	Vector interp;

	// 0.0
	interpArray[0] = 3.1f * 1.0f + 7.2f * 0.0f;
	interpArray[1] = 3.4f * 1.0f + 0.6f * 0.0f;
	interpArray[2] = 3.7f * 1.0f + 4.8f * 0.0f;
	interpArray[3] = 4.0f * 1.0f + 5.1f * 0.0f;
	interpArray[4] = 4.3f * 1.0f + 8.9f * 0.0f;
	interp = Vector(interpArray);
	EXPECT_TRUE(interp.isApprox(prev));
	EXPECT_TRUE(interp.isApprox(SurgSim::Math::interpolate(prev, next, static_cast<T>(0.0f)), epsilon));

	// 1.0
	interpArray[0] = 3.1f * 0.0f + 7.2f * 1.0f;
	interpArray[1] = 3.4f * 0.0f + 0.6f * 1.0f;
	interpArray[2] = 3.7f * 0.0f + 4.8f * 1.0f;
	interpArray[3] = 4.0f * 0.0f + 5.1f * 1.0f;
	interpArray[4] = 4.3f * 0.0f + 8.9f * 1.0f;
	interp = Vector(interpArray);
	EXPECT_TRUE(interp.isApprox(next));
	EXPECT_TRUE(interp.isApprox(SurgSim::Math::interpolate(prev, next, static_cast<T>(1.0f)), epsilon));

	// 0.5
	interpArray[0] = 3.1f * 0.5f + 7.2f * 0.5f;
	interpArray[1] = 3.4f * 0.5f + 0.6f * 0.5f;
	interpArray[2] = 3.7f * 0.5f + 4.8f * 0.5f;
	interpArray[3] = 4.0f * 0.5f + 5.1f * 0.5f;
	interpArray[4] = 4.3f * 0.5f + 8.9f * 0.5f;
	interp = Vector(interpArray);
	EXPECT_TRUE(interp.isApprox(SurgSim::Math::interpolate(prev, next, static_cast<T>(0.5f)), epsilon));

	// 0.886
	interpArray[0] = 3.1f * 0.114f + 7.2f * 0.886f;
	interpArray[1] = 3.4f * 0.114f + 0.6f * 0.886f;
	interpArray[2] = 3.7f * 0.114f + 4.8f * 0.886f;
	interpArray[3] = 4.0f * 0.114f + 5.1f * 0.886f;
	interpArray[4] = 4.3f * 0.114f + 8.9f * 0.886f;
	interp = Vector(interpArray);
	EXPECT_TRUE(interp.isApprox(SurgSim::Math::interpolate(prev, next, static_cast<T>(0.886f)), epsilon));

	// 0.623
	interpArray[0] = 3.1f * 0.377f + 7.2f * 0.623f;
	interpArray[1] = 3.4f * 0.377f + 0.6f * 0.623f;
	interpArray[2] = 3.7f * 0.377f + 4.8f * 0.623f;
	interpArray[3] = 4.0f * 0.377f + 5.1f * 0.623f;
	interpArray[4] = 4.3f * 0.377f + 8.9f * 0.623f;
	interp = Vector(interpArray);
	EXPECT_TRUE(interp.isApprox(SurgSim::Math::interpolate(prev, next, static_cast<T>(0.623f)), epsilon));
}


// TO DO:
// testing numerical validity
// testing for denormalized numbers
// testing degeneracy (norm near 0)
// compute an orthonormal frame based on a given normal (z-axis)

TYPED_TEST(AllDynamicVectorTests, CanResize)
{
	typedef typename TestFixture::Vector Vector;

	ASSERT_NO_THROW({Vector a; a.resize(10);});
}

namespace
{
	template <class T>
	void testScalar(T valueExpected, T value){}

	template <>
	void testScalar<double>(double valueExpected, double value)
	{
		EXPECT_DOUBLE_EQ(valueExpected, value);
	}

	template <>
	void testScalar<float>(float valueExpected, float value)
	{
		EXPECT_FLOAT_EQ(valueExpected, value);
	}
};

TYPED_TEST(AllDynamicVectorTests, addSubVector)
{
	typedef typename TestFixture::Vector Vector;

	Vector v, vInit, v2, v2Init;
	v.resize(18);   v.setRandom();   vInit  = v;
	v2.resize(18);  v2.setRandom();  v2Init = v2;

	ASSERT_NO_THROW(SurgSim::Math::addSubVector(v2.segment(3,3), 2, 3, &v););
	EXPECT_TRUE(v2.isApprox(v2Init));
	EXPECT_FALSE(v.isApprox(vInit));
	for (int dofId = 0; dofId < 6; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
	for (int dofId = 6; dofId < 9; dofId++)
	{
		testScalar(vInit[dofId] + v2Init[3 + dofId-6], v[dofId]);
	}
	for (int dofId = 9; dofId < 18; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
}

TYPED_TEST(AllDynamicVectorTests, addSubVectorBlocks)
{
	typedef typename TestFixture::Vector Vector;

	Vector v, vInit, v2, v2Init;
	std::vector<unsigned int> nodeIds;
	v.resize(18);   v.setRandom();   vInit = v;
	v2.resize(18);  v2.setRandom();  v2Init = v2;
	nodeIds.push_back(1);
	nodeIds.push_back(3);
	nodeIds.push_back(5);

	ASSERT_NO_THROW(SurgSim::Math::addSubVector(v2.segment(3,15), nodeIds, 3, &v););
	EXPECT_TRUE(v2.isApprox(v2Init));
	EXPECT_FALSE(v.isApprox(vInit));
	for (int dofId = 0; dofId < 3; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
	for (int dofId = 3; dofId < 6; dofId++)
	{
		testScalar(vInit[dofId] + v2Init[3 + (dofId - 3)], v[dofId]);
	}
	for (int dofId = 6; dofId < 9; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
	for (int dofId = 9; dofId < 12; dofId++)
	{
		testScalar(vInit[dofId] + v2Init[3 + (dofId - 6)], v[dofId]);
	}
	for (int dofId = 12; dofId < 15; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
	for (int dofId = 15; dofId < 18; dofId++)
	{
		testScalar(vInit[dofId] + v2Init[3 + (dofId - 9)], v[dofId]);
	}
}

TYPED_TEST(AllDynamicVectorTests, setSubVector)
{
	typedef typename TestFixture::Vector Vector;

	Vector v, vInit, v2, v2Init;
	v.resize(18);   v.setRandom();   vInit  = v;
	v2.resize(18);  v2.setRandom();  v2Init = v2;

	ASSERT_NO_THROW(SurgSim::Math::setSubVector(v2.segment(3,3), 2, 3, &v););
	EXPECT_TRUE(v2.isApprox(v2Init));
	EXPECT_FALSE(v.isApprox(vInit));
	for (int dofId = 0; dofId < 6; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
	for (int dofId = 6; dofId < 9; dofId++)
	{
		testScalar(v2Init[3 + dofId-6], v[dofId]);
	}
	for (int dofId = 9; dofId < 18; dofId++)
	{
		testScalar(vInit[dofId], v[dofId]);
	}
}

TYPED_TEST(AllDynamicVectorTests, getSubVector)
{
	typedef typename TestFixture::Vector Vector;

	Vector v, vInit;
	v.resize(18); v.setRandom(); vInit = v;

	Eigen::VectorBlock<Vector> subVector = SurgSim::Math::getSubVector(v, 2, 3);
	EXPECT_TRUE(v.isApprox(vInit));
	for (int dofId = 0; dofId < 3; dofId++)
	{
		testScalar(v[2 * 3 + dofId], subVector[dofId]);
		// Also test that the returned value are pointing to the correct data
		EXPECT_EQ(&subVector[dofId], &v[2 * 3 + dofId]);
	}
}

TYPED_TEST(AllDynamicVectorTests, getSubVectorBlocks)
{
	typedef typename TestFixture::Vector Vector;

	Vector v, vInit, v2;
	std::vector<unsigned int> nodeIds;
	v.resize(18);   v.setRandom();   vInit = v;
	v2.resize(9);  v2.setZero();
	nodeIds.push_back(1);
	nodeIds.push_back(3);
	nodeIds.push_back(5);

	EXPECT_EQ(18, v.size());
	EXPECT_TRUE(v.isApprox(vInit));
	EXPECT_EQ(9, v2.size());
	EXPECT_TRUE(v2.isZero());
	ASSERT_NO_THROW(SurgSim::Math::getSubVector(v, nodeIds, 3, &v2););
	EXPECT_EQ(18, v.size());
	EXPECT_TRUE(v.isApprox(vInit));
	EXPECT_EQ(9, v2.size());
	EXPECT_FALSE(v2.isZero());
	for (int dofId = 0; dofId < 3; dofId++)
	{
		testScalar(v[3 + dofId], v2[dofId]);
	}
	for (int dofId = 3; dofId < 6; dofId++)
	{
		testScalar(v[9 + (dofId - 3)], v2[dofId]);
	}
	for (int dofId = 6; dofId < 9; dofId++)
	{
		testScalar(v[15 + (dofId - 6)], v2[dofId]);
	}
}

TYPED_TEST(AllDynamicVectorTests, resize)
{
	typedef typename TestFixture::Vector Vector;

	Vector v;

	ASSERT_NO_THROW(SurgSim::Math::resize(&v, 10, false););
	EXPECT_EQ(10, static_cast<int>(v.size()));

	ASSERT_NO_THROW(SurgSim::Math::resize(&v, 13, true););
	EXPECT_EQ(13, static_cast<int>(v.size()));
	EXPECT_TRUE(v.isZero());
}

template <class Vector>
void testOrthonormalBasis(const Vector& i, const Vector& j, const Vector& k)
{
	typedef typename Vector::Scalar T;

	T precision = Eigen::NumTraits<T>::dummy_precision();

	EXPECT_NEAR(i.dot(j), 0.0, precision);
	EXPECT_NEAR(i.dot(k), 0.0, precision);
	EXPECT_NEAR(j.dot(i), 0.0, precision);
	EXPECT_NEAR(j.dot(k), 0.0, precision);
	EXPECT_NEAR(k.dot(i), 0.0, precision);
	EXPECT_NEAR(k.dot(j), 0.0, precision);

	EXPECT_TRUE(i.cross(j).isApprox(k));
	EXPECT_TRUE(j.cross(k).isApprox(i));
	EXPECT_TRUE(k.cross(i).isApprox(j));

	EXPECT_NEAR(i.norm(), 1.0, precision);
	EXPECT_NEAR(j.norm(), 1.0, precision);
	EXPECT_NEAR(k.norm(), 1.0, precision);
}

TYPED_TEST(Vector3Tests, buildOrthonormalBasis)
{
	typedef typename TestFixture::Vector3 Vector3;
	typedef typename Vector3::Scalar T;
	const int VOpt = Vector3::Options;

	Vector3 i(static_cast<T>(1.54), static_cast<T>(-4.25), static_cast<T>(0.983));
	Vector3 j, k;
	T precision = Eigen::NumTraits<T>::dummy_precision();

	// Assert if 1 parameter is nullptr
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(nullptr, &j, &k)));
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(&i, nullptr, &k)));
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(&i, &j, nullptr)));

	// Assert if 2 parameters are nullptr
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(nullptr, nullptr, &k)));
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(nullptr, &j, nullptr)));
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(&i, nullptr, nullptr)));

	// Assert if 3 parameters are nullptr
	ASSERT_ANY_THROW((SurgSim::Math::buildOrthonormalBasis<T, VOpt>(nullptr, nullptr, nullptr)));

	// Input parameter 'i' = (0, 0, 0)
	Vector3 zero = Vector3::Zero();
	ASSERT_NO_THROW(EXPECT_EQ(false, SurgSim::Math::buildOrthonormalBasis(&zero, &j, &k)));

	// Input parameter 'i' = (0, 0, 0) + (epsilon, epsilon, epsilon)
	Vector3 closeToZero = Vector3::Constant(precision);
	ASSERT_NO_THROW(EXPECT_EQ(false, SurgSim::Math::buildOrthonormalBasis(&closeToZero, &j, &k)));

	// Input parameter 'i' is already normalized
	{
		Vector3 i(static_cast<T>(1.54), static_cast<T>(-4.25), static_cast<T>(0.983));
		Vector3 j, k;

		i.normalize();
		ASSERT_NO_THROW(EXPECT_EQ(true, SurgSim::Math::buildOrthonormalBasis(&i, &j, &k)));
		testOrthonormalBasis(i, j, k);
	}

	// Input parameter 'i' is not already normalized
	{
		Vector3 i(static_cast<T>(1.54), static_cast<T>(-4.25), static_cast<T>(0.983));
		Vector3 j, k;

		ASSERT_NO_THROW(EXPECT_EQ(true, SurgSim::Math::buildOrthonormalBasis(&i, &j, &k)));
		testOrthonormalBasis(i, j, k);
	}
}
