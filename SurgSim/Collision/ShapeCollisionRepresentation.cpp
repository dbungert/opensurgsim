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

#include "SurgSim/Collision/ShapeCollisionRepresentation.h"
#include "SurgSim/Math/MeshShape.h"
#include "SurgSim/Physics/Representation.h"

namespace SurgSim
{
namespace Collision
{


ShapeCollisionRepresentation::ShapeCollisionRepresentation(
		const std::string& name,
		std::shared_ptr<SurgSim::Math::Shape> shape,
		const SurgSim::Math::RigidTransform3d& pose) :
	Representation(name),
	m_shape(shape)
{
	setLocalPose(pose);
	update(0.0);
}

ShapeCollisionRepresentation::~ShapeCollisionRepresentation()
{
}

int ShapeCollisionRepresentation::getShapeType() const
{
	return m_shape->getType();
}

const std::shared_ptr<SurgSim::Math::Shape> ShapeCollisionRepresentation::getShape() const
{
	return m_shape;
}

void ShapeCollisionRepresentation::update(const double& dt)
{
	Representation::update(dt);

	auto meshShape = std::dynamic_pointer_cast<SurgSim::Math::MeshShape>(m_shape);
	if (meshShape != nullptr)
	{
		meshShape->setPose(getPose());
	}
}

}; // namespace Collision
}; // namespace SurgSim
