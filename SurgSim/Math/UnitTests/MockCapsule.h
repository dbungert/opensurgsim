// This file is a part of the OpenSurgSim project.
// Copyright 2013, SimQuest Solutions Inc.
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

#ifndef SURGSIM_MATH_UNITTESTS_MOCKCAPSULE_H
#define SURGSIM_MATH_UNITTESTS_MOCKCAPSULE_H

#include "SurgSim/Math/Vector.h"
#include "SurgSim/Math/RigidTransform.h"

namespace SurgSim
{
namespace Math
{

/// MockCapsule class used in the unit tests.
class MockCapsule
{
public:
	// Default constructor.
	MockCapsule() : r(std::numeric_limits<double>::signaling_NaN())
	{}

	// Constructor.
	MockCapsule(const Vector3d& vertex0, const Vector3d& vertex1, double radius) :
		v0(vertex0), v1(vertex1), v0v1(vertex1 - vertex0), r(radius)
	{}

	// Move this capsule by the given vector.
	void translate(const Vector3d& v)
	{
		v0 += v;
		v1 += v;
	}

	// Rotate this capsule about the x-axis by the given angle.
	void rotateAboutXBy(double angle)
	{
		RigidTransform3d r(Eigen::AngleAxis<double>(angle * (M_PI / 180.0), Vector3d(1, 0, 0)));
		v0  = r * v0;
		v1  = r * v1;
	}

	// Rotate this capsule about the y-axis by the given angle.
	void rotateAboutYBy(double angle)
	{
		SurgSim::Math::RigidTransform3d r(Eigen::AngleAxis<double>(angle * (M_PI / 180.0), Vector3d(0, 1, 0)));
		v0  = r * v0;
		v1  = r * v1;
	}

	// Rotate this capsule about the z-axis by the given angle.
	void rotateAboutZBy(double angle)
	{
		SurgSim::Math::RigidTransform3d r(Eigen::AngleAxis<double>(angle * (M_PI / 180.0), Vector3d(0, 0, 1)));
		v0  = r * v0;
		v1  = r * v1;
	}

	// Transform this capsule by the given matrix.
	void transform(SurgSim::Math::RigidTransform3d transform)
	{
		v0 = transform * v0;
		v1 = transform * v1;
	}

	// Vertices of this capsule.
	Vector3d v0;
	Vector3d v1;

	// Axis of this capsule.
	Vector3d v0v1;

	// Radius
	double r;
};


} // namespace Math

} // namespace SurgSim

#endif // SURGSIM_MATH_UNITTESTS_MOCKCAPSULE_H