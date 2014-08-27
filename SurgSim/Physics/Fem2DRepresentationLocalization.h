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

#ifndef SURGSIM_PHYSICS_FEM2DREPRESENTATIONLOCALIZATION_H
#define SURGSIM_PHYSICS_FEM2DREPRESENTATIONLOCALIZATION_H

#include "SurgSim/DataStructures/IndexedLocalCoordinate.h"
#include "SurgSim/Physics/Localization.h"

namespace SurgSim
{

namespace Physics
{

/// Implementation of Localization for Fem2DRepresentation
///
/// Fem2DRepresentationLocalization tracks the global coordinates of an IndexedLocalCoordinate associated with an
/// Fem2DRepresentation.
///
/// Fem2DRepresentationLocalization stores a pointer to a Fem2DRepresentation in an abstract Representation object.  It
/// holds an IndexedLocalCoordinate, which contains a barycentric coordinate and the ID of an FemElement associated
/// with an Fem2DRepresentation, and it provides a helper function Fem2DRepresentationLocalization::calculatePosition to
/// find the IndexedLocalCoordinate in coordinates in the current OdeState.
class Fem2DRepresentationLocalization : public Localization
{
public:
	/// Default constructor
	Fem2DRepresentationLocalization();

	/// Constructor
	/// \param representation The representation to assign to this localization.
	explicit Fem2DRepresentationLocalization(std::shared_ptr<Representation> representation);

	/// Destructor
	virtual ~Fem2DRepresentationLocalization();

	/// Sets the local position.
	/// \param p The local position to set the localization at.
	void setLocalPosition(const SurgSim::DataStructures::IndexedLocalCoordinate& p);

	/// Gets the local position.
	/// \return The local position set for this localization.
	const SurgSim::DataStructures::IndexedLocalCoordinate& getLocalPosition() const;

	/// Query if 'representation' is valid representation.
	/// \param	representation	The representation.
	/// \return	true if valid representation, false if not.
	virtual bool isValidRepresentation(std::shared_ptr<Representation> representation) override;

private:
	/// Calculates the global position of this localization.
	/// \param time The time in [0..1] at which the position should be calculated.
	/// \return The global position of the localization at the requested time.
	/// \note time can be useful when dealing with CCD.
	SurgSim::Math::Vector3d doCalculatePosition(double time);

	/// Barycentric position in local coordinates
	SurgSim::DataStructures::IndexedLocalCoordinate m_position;
};

} // namespace Physics

} // namespace SurgSim

#endif // SURGSIM_PHYSICS_FEM2DREPRESENTATIONLOCALIZATION_H
