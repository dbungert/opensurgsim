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

#include "SurgSim/Framework/Assert.h"
#include "SurgSim/Physics/Fem2DRepresentation.h"

namespace
{
void transformVectorByBlockOf3(const SurgSim::Math::RigidTransform3d& transform, SurgSim::Math::Vector* x,
							   bool rotationOnly = false)
{
	typedef SurgSim::Math::Vector::Index IndexType;

	IndexType numNodes = x->size() / 6;

	SURGSIM_ASSERT(numNodes * 6 == x->size())
		<< "Unexpected number of dof in a Fem2D state vector (not a multiple of 6)";

	for (IndexType nodeId = 0; nodeId < numNodes; nodeId++)
	{
		// Only the translational dof are transformed, rotational dof remains unchanged
		SurgSim::Math::Vector3d xi = x->segment<3>(6 * nodeId);

		x->segment<3>(6 * nodeId) = (rotationOnly) ? transform.linear() * xi : transform * xi;
	}
}
}

namespace SurgSim
{

namespace Physics
{

Fem2DRepresentation::Fem2DRepresentation(const std::string& name) : FemRepresentation(name)
{
	// Reminder: m_numDofPerNode is held by DeformableRepresentation but needs to be set by all concrete derived classes
	m_numDofPerNode = 6;
}

Fem2DRepresentation::~Fem2DRepresentation()
{
}

RepresentationType Fem2DRepresentation::getType() const
{
	return REPRESENTATION_TYPE_FEM2D;
}

void Fem2DRepresentation::transformState(std::shared_ptr<SurgSim::Math::OdeState> state,
										 const SurgSim::Math::RigidTransform3d& transform)
{
	transformVectorByBlockOf3(transform, &state->getPositions());
	transformVectorByBlockOf3(transform, &state->getVelocities(), true);
}

} // namespace Physics

} // namespace SurgSim
