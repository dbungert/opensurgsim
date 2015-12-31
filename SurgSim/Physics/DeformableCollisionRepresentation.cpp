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

#include "SurgSim/DataStructures/TriangleMesh.h"
#include "SurgSim/Framework/ObjectFactory.h"
#include "SurgSim/Framework/Runtime.h"
#include "SurgSim/Framework/SceneElement.h"
#include "SurgSim/Math/MathConvert.h"
#include "SurgSim/Math/MeshShape.h"
#include "SurgSim/Math/OdeState.h"
#include "SurgSim/Math/SegmentMeshShape.h"
#include "SurgSim/Math/Shape.h"
#include "SurgSim/Physics/DeformableCollisionRepresentation.h"
#include "SurgSim/Physics/DeformableRepresentation.h"

namespace SurgSim
{
namespace Physics
{
SURGSIM_REGISTER(SurgSim::Framework::Component, SurgSim::Physics::DeformableCollisionRepresentation,
				 DeformableCollisionRepresentation);

DeformableCollisionRepresentation::DeformableCollisionRepresentation(const std::string& name) :
	SurgSim::Collision::Representation(name)
{
	SURGSIM_ADD_SERIALIZABLE_PROPERTY(DeformableCollisionRepresentation, std::shared_ptr<SurgSim::Math::Shape>,
									  Shape, getShape, setShape);
}

DeformableCollisionRepresentation::~DeformableCollisionRepresentation()
{
}

namespace
{
bool updateShapeFromOdeState(const Math::OdeState& odeState, SurgSim::Math::Shape* shape)
{
	auto vertices = dynamic_cast<SurgSim::DataStructures::Vertices<SurgSim::DataStructures::EmptyData>*>(shape);
	SURGSIM_ASSERT(vertices != nullptr) << "The Shape is not a Vertices.";
	const size_t numNodes = odeState.getNumNodes();
	SURGSIM_ASSERT(vertices->getNumVertices() == numNodes) <<
		"The number of nodes in the deformable does not match the number of vertices in the shape.";

	for (size_t nodeId = 0; nodeId < numNodes; ++nodeId)
	{
		vertices->setVertexPosition(nodeId, odeState.getPosition(nodeId));
	}
	return vertices->update();
}
}

void DeformableCollisionRepresentation::update(const double& dt)
{
	auto physicsRepresentation = m_deformable.lock();
	SURGSIM_ASSERT(nullptr != physicsRepresentation) <<
		"Failed to update.  The DeformableCollisionRepresentation either was not attached to a "
		"Physics::Representation or the Physics::Representation has expired.";

	// Write previous shape ONLY if the collision detection is continuous
	if (getCollisionDetectionType() == Collision::COLLISION_DETECTION_TYPE_CONTINUOUS)
	{
		if (m_previousShape == nullptr)
		{
			if (m_shape->getType() == SurgSim::Math::SHAPE_TYPE_MESH)
			{
				m_previousShape = std::make_shared<Math::MeshShape>(*std::dynamic_pointer_cast<Math::MeshShape>(m_shape));
			}
			else if (m_shape->getType() == SurgSim::Math::SHAPE_TYPE_SEGMENTMESH)
			{
				m_previousShape = std::make_shared<Math::SegmentMeshShape>(*std::dynamic_pointer_cast<Math::SegmentMeshShape>(m_shape));
			}
			else
			{
				SURGSIM_FAILURE() << "Invalid type, should be MeshShape("<< SurgSim::Math::SHAPE_TYPE_MESH <<
					") or SegmentMeshShape("<< SurgSim::Math::SHAPE_TYPE_SEGMENTMESH<<"), but it is " <<
					m_shape->getType();
			}
		}
		if (!updateShapeFromOdeState(*physicsRepresentation->getPreviousState().get(), m_previousShape.get()))
		{
			setLocalActive(false);
			SURGSIM_LOG_SEVERE(SurgSim::Framework::Logger::getLogger("Collision/DeformableCollisionRepresentation")) <<
				"CollisionRepresentation '" << getFullName() << "' went inactive because its shape failed to update.";
		}
	}

	// Write current shape
	if (!updateShapeFromOdeState(*physicsRepresentation->getCurrentState().get(), m_shape.get()))
	{
		setLocalActive(false);
		SURGSIM_LOG_SEVERE(Framework::Logger::getLogger("Collision/DeformableCollisionRepresentation")) <<
			"CollisionRepresentation '" << getFullName() << "' went inactive because its shape failed to update.";
	}

	Math::PosedShape<std::shared_ptr<Math::Shape>> posedShapeFirst(m_previousShape, Math::RigidTransform3d::Identity());
	Math::PosedShape<std::shared_ptr<Math::Shape>> posedShapeSecond(m_shape, Math::RigidTransform3d::Identity());
	Math::PosedShapeMotion<std::shared_ptr<Math::Shape>> posedShapeMotion(posedShapeFirst, posedShapeSecond);
	setPosedShapeMotion(posedShapeMotion);
}

bool DeformableCollisionRepresentation::doInitialize()
{
	bool result = false;
	if (nullptr != m_shape && m_shape->isValid())
	{
		result = true;
	}

	return result;
}

bool DeformableCollisionRepresentation::doWakeUp()
{
	auto physicsRepresentation = m_deformable.lock();
	SURGSIM_ASSERT(nullptr != physicsRepresentation) <<
		"The Physics::Representation referred by this DeformableCollisionRepresentation has expired.";

	auto state = physicsRepresentation->getCurrentState();
	SURGSIM_ASSERT(nullptr != state) <<
		"DeformableRepresentation " << physicsRepresentation->getName() << " holds an empty OdeState.";
	auto shape = std::dynamic_pointer_cast<DataStructures::VerticesPlain>(m_shape);
	SURGSIM_ASSERT(shape != nullptr) <<
		"The shape object is not inherited from DataStructures::VerticesPlain, but should be.";
	SURGSIM_ASSERT(shape->getNumVertices() == state->getNumNodes()) <<
		"The number of nodes in the deformable does not match the number of vertices in the mesh.";

	update(0.0);
	return true;
}

int DeformableCollisionRepresentation::getShapeType() const
{
	SURGSIM_ASSERT(nullptr != m_shape) << "No mesh/shape assigned to DeformableCollisionRepresentation " << getName();
	return m_shape->getType();
}

void DeformableCollisionRepresentation::setShape(std::shared_ptr<SurgSim::Math::Shape> shape)
{
	SURGSIM_ASSERT(shape->getType() == SurgSim::Math::SHAPE_TYPE_MESH ||
		shape->getType() == SurgSim::Math::SHAPE_TYPE_SEGMENTMESH)
		<< "Deformable collision shape has to be a mesh.  But what passed in is " << shape->getType();

	m_shape = shape;
}

const std::shared_ptr<SurgSim::Math::Shape> DeformableCollisionRepresentation::getShape() const
{
	return m_shape;
}

void DeformableCollisionRepresentation::setDeformableRepresentation(
	std::shared_ptr<SurgSim::Physics::DeformableRepresentation>representation)
{
	m_deformable = representation;
}

const std::shared_ptr<SurgSim::Physics::DeformableRepresentation>
	DeformableCollisionRepresentation::getDeformableRepresentation() const
{
	auto physicsRepresentation = m_deformable.lock();
	SURGSIM_ASSERT(physicsRepresentation != nullptr) <<
		"Failed to get the deformable representation.  The DeformableCollisionRepresentation either was not "
		"attached to a Physics::Representation or the Physics::Representation has expired.";

	return physicsRepresentation;
}

} // namespace Physics
} // namespace SurgSim
