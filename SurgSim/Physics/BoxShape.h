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

#ifndef SURGSIM_PHYSICS_BOXSHAPE_H
#define SURGSIM_PHYSICS_BOXSHAPE_H

#include <SurgSim/Physics/RigidShape.h>
#include <SurgSim/Math/Quaternion.h>

namespace SurgSim
{

namespace Physics
{

/// Box shape: box centered on (0 0 0), aligned with the axis
/// with different sizes along X, Y and Z
class BoxShape: public RigidShape
{
public:
	/// Constructor
	/// \param sizeX, sizeY, sizeZ the box sizes in all 3 directions (in m)
	BoxShape(double sizeX, double sizeY, double sizeZ);

	/// \return the type of the shape
	virtual int getType();

	/// Get size in X direction
	/// \return the size in the X direction (in m)
	double getSizeX() const;

	/// Get size in Y direction
	/// \return the size in the Y direction (in m)
	double getSizeY() const;

	/// Get size in Z direction
	/// \return the size in the Z direction (in m)
	double getSizeZ() const;

	/// Calculate the volume of the box
	/// \return The volume of the box (in m-3)
	virtual double calculateVolume() const;

	/// Calculate the mass center of the box
	/// \return The mass center of the box
	virtual Vector3d calculateMassCenter() const;

	/// Calculate the inertia of the box
	/// \param rho The mass density (in Kg.m-3)
	/// \return The 3x3 symmetric inertia matrix of the box
	virtual Matrix33d calculateInertia(double rho) const;

	/// Function that calculates the global vertex locations, given an orientation
	/// and translation.
	/// \param i The vertex index.
	/// \param quat The orientation of the box.
	/// \param trans The translation of the box.
	/// \return The global vertex position.
	Vector3d calculateGlobalVertex(const int i,
								   const SurgSim::Math::Quaterniond& quat,
								   const Vector3d& trans) const;

	/// Function that returns the local vertex location, given an index.
	/// and translation.
	/// \param i The vertex index.
	/// \return The local vertex position.
	Vector3d getLocalVertex(const int i) const;

private:
	/// Function that calculates the box vertices.
	void calculateVertices();

	/// The box sizes along the 3 axis respectively {X,Y,Z}
	double   m_size[3];

	/// The box vertices.
	Vector3d m_vertices[8];
};

}; // Physics

}; // SurgSim

#endif // SURGSIM_PHYSICS_BOXSHAPE_H
