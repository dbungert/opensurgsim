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

#ifndef SURGSIM_PHYSICS_MASSSPRINGLOCALIZATION_H
#define SURGSIM_PHYSICS_MASSSPRINGLOCALIZATION_H

#include "SurgSim/DataStructures/IndexedLocalCoordinate.h"
#include "SurgSim/DataStructures/OptionalValue.h"
#include "SurgSim/Physics/Localization.h"

namespace SurgSim
{
namespace Physics
{
class MassSpringRepresentation;

/// Implementation of Localization for MassSpringRepresentation
///
/// MassSpringLocalization tracks the global coordinates of a node contained in an associated
/// MassSpringRepresentation.  It is used, for example, as a helper class for filling out the MlcpPhysicsProblem in
/// MassSpringRepresentationContact::doBuild, which constrains the motion of MassSpringRepresentation at a frictionless
/// contact.
///
/// MassSpringLocalization stores a pointer to a MassSpringRepresentation in an abstract Representation
/// object.  It tracks either the ID of a node contained within the associated MassSpringRepresentation, or the
/// barycentric coordinates of an element (1D, 2D, or 3D) in that MassSpringRepresentation, and it provides a
/// helper function MassSpringLocalization::calculatePosition to find the position in global
/// coordinates in the current state.
class MassSpringLocalization: public Localization
{
public:
	/// Default constructor
	MassSpringLocalization();

	/// Constructor
	/// \param representation The representation to assign to this localization.
	explicit MassSpringLocalization(std::shared_ptr<Representation> representation);

	/// Destructor
	virtual ~MassSpringLocalization();

	/// Sets the local node.
	/// \param nodeID Node set for this localization.
	void setLocalNode(size_t nodeID);

	/// Gets the local node.
	/// \return Node set for this localization.
	const DataStructures::OptionalValue<size_t>& getLocalNode() const;

	/// Sets the local position.
	/// \param localPosition The local position to set the localization at.
	void setLocalPosition(const SurgSim::DataStructures::IndexedLocalCoordinate& localPosition);

	/// Gets the local position.
	/// \return The local position set for this localization.
	const DataStructures::OptionalValue<SurgSim::DataStructures::IndexedLocalCoordinate>& getLocalPosition() const;

	/// Queries whether Representation can be assigned to this class.
	/// \param representation Representation to check.
	/// \return	true if Representation is valid.
	bool isValidRepresentation(std::shared_ptr<Representation> representation) override;

	bool moveClosestTo(const Math::Vector3d& point, bool *hasReachedEnd) override;

private:
	/// Calculates the global position of this localization.
	/// \param time Interpolation parameter [0..1] for calcuting position between the previous state (0.0) and current
	/// state (1.0).
	/// \return The global position of the localization using an interpolation between the previous and current states.
	/// \note The time parameter can useful when dealing with Continuous Collision Detection.
	SurgSim::Math::Vector3d doCalculatePosition(double time) const override;

	SurgSim::Math::Vector3d doCalculateVelocity(double time) const override;

	/// Node defining the localization.
	DataStructures::OptionalValue<size_t> m_nodeID;
	/// Barycentric position in local coordinates
	DataStructures::OptionalValue<DataStructures::IndexedLocalCoordinate> m_position;
};

};  // namespace Physics

};  // namespace SurgSim

#endif  // SURGSIM_PHYSICS_MASSSPRINGLOCALIZATION_H
