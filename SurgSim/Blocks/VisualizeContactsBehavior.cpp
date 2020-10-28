// This file is a part of the OpenSurgSim project.
// Copyright 2013-2015, SimQuest Solutions Inc.
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

#include <algorithm>

#include "SurgSim/Blocks/VisualizeContactsBehavior.h"
#include "SurgSim/Collision/CollisionPair.h"
#include "SurgSim/Collision/Representation.h"
#include "SurgSim/DataStructures/BufferedValue.h"
#include "SurgSim/DataStructures/Vertex.h"
#include "SurgSim/Framework/FrameworkConvert.h"
#include "SurgSim/Framework/Log.h"
#include "SurgSim/Framework/SceneElement.h"
#include "SurgSim/Graphics/OsgVectorFieldRepresentation.h"
#include "SurgSim/Graphics/VectorField.h"
#include "SurgSim/Math/RigidTransform.h"

using SurgSim::Collision::Contact;
using SurgSim::Collision::Representation;
using SurgSim::DataStructures::Vertex;
using SurgSim::Framework::checkAndConvert;
using SurgSim::Graphics::OsgVectorFieldRepresentation;
using SurgSim::Graphics::VectorField;
using SurgSim::Graphics::VectorFieldData;


namespace SurgSim
{

namespace Blocks
{

SURGSIM_REGISTER(SurgSim::Framework::Component, SurgSim::Blocks::VisualizeContactsBehavior,
				 VisualizeContactsBehavior);

VisualizeContactsBehavior::VisualizeContactsBehavior(const std::string& name):
	SurgSim::Framework::Behavior(name),
	m_vectorField(std::make_shared<OsgVectorFieldRepresentation>(name + std::string("_VectorField")))
{
	SURGSIM_ADD_SERIALIZABLE_PROPERTY(VisualizeContactsBehavior, std::shared_ptr<SurgSim::Framework::Component>,
									  Source, getSource, setSource);
	SURGSIM_ADD_SERIALIZABLE_PROPERTY(VisualizeContactsBehavior, double, VectorFieldScale,
									  getVectorFieldScale, setVectorFieldScale);
}

std::shared_ptr<SurgSim::Framework::Component> VisualizeContactsBehavior::getSource()
{
	return m_source;
}

void VisualizeContactsBehavior::setSource(std::shared_ptr<SurgSim::Framework::Component> source)
{
	m_source = checkAndConvert<Representation>(source, "SurgSim::Collision::Representation");
}

void VisualizeContactsBehavior::update(double dt)
{
	std::shared_ptr<const SurgSim::Collision::ContactMapType> collisions =
		m_source->getCollisions().safeGet();
	if (!collisions->empty())
	{
		size_t totalContacts = 0;
		for (auto collision = collisions->cbegin(); collision != collisions->cend(); ++collision)
		{
			totalContacts += collision->second.size();
		}

		std::shared_ptr<VectorField> vectorField = m_vectorField->getVectorField();
		vectorField->clear();
		vectorField->getVertices().reserve(2 * totalContacts);

		Math::RigidTransform3d inverseElementPose;
		auto element = getSceneElement();
		if (element != nullptr)
		{
			inverseElementPose = element->getPose().inverse();
		}
		else
		{
			inverseElementPose = Math::RigidTransform3d::Identity();
			SURGSIM_LOG_ONCE(Framework::Logger::getDefaultLogger(), WARNING) << getClassName() << " named '"
				<< getFullName() << "' must be in a SceneElement.";
		}

		auto representationPoseFirst = m_source->getPose();
		for (auto it = collisions->cbegin(); it != collisions->cend(); ++it)
		{
			auto representationPoseSecond = (*it).first->getPose();
			for (auto iter = (*it).second.cbegin(); iter != (*it).second.cend(); ++iter)
			{
				VectorFieldData vectorData1;
				VectorFieldData vectorData2;
				vectorData2.direction = inverseElementPose.linear() * (*iter)->normal * (*iter)->depth;
				vectorData1.direction = -vectorData2.direction;

				Vertex<VectorFieldData> vertex1 = Vertex<VectorFieldData>(
						(*iter)->penetrationPoints.first.rigidLocalPosition.getValue(), vectorData1);
				Vertex<VectorFieldData> vertex2 = Vertex<VectorFieldData>(
						(*iter)->penetrationPoints.second.rigidLocalPosition.getValue(), vectorData2);

				vertex1.position = inverseElementPose * representationPoseFirst * vertex1.position;
				vertex2.position = inverseElementPose * representationPoseSecond * vertex2.position;
				vertex1.data.color = Math::Vector4d(1.0, 0.0, 0.0, 1.0);
				vertex2.data.color = Math::Vector4d(0.0, 0.0, 1.0, 1.0);
				vectorField->addVertex(vertex1);
				vectorField->addVertex(vertex2);
			}
		}
		m_vectorField->setLocalActive(true);
	}
	else
	{
		m_vectorField->setLocalActive(false);
	}
}

int VisualizeContactsBehavior::getTargetManagerType() const
{
	return SurgSim::Framework::MANAGER_TYPE_GRAPHICS;
}

bool VisualizeContactsBehavior::doInitialize()
{
	return true;
}

bool VisualizeContactsBehavior::doWakeUp()
{
	if (m_source == nullptr)
	{
		SURGSIM_LOG_SEVERE(SurgSim::Framework::Logger::getDefaultLogger()) << getClassName() << " named '"
			<< getName() << "' must have a source.";
		return false;
	}

	auto element = getSceneElement();
	if (element == nullptr)
	{
		SURGSIM_LOG_SEVERE(SurgSim::Framework::Logger::getDefaultLogger()) << getClassName() << " named '"
				<< getFullName() << "' must be in a SceneElement.";
		return false;
	}

	return element->addComponent(m_vectorField);
}

double VisualizeContactsBehavior::getVectorFieldScale()
{
	return m_vectorField->getScale();
}

void VisualizeContactsBehavior::setVectorFieldScale(double scale)
{
	SURGSIM_ASSERT(scale > 0.0) << "Scale of vector field must be positive.";
	m_vectorField->setScale(scale);
}

} // namespace Blocks
} // namespace SurgSim
