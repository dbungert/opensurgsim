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

#include "SurgSim/Physics/Fem1DRepresentationLocalization.h"

#include "SurgSim/Math/Vector.h"
#include "SurgSim/Physics/Fem1DRepresentation.h"
#include "SurgSim/Physics/FemElement.h"

namespace SurgSim
{

namespace Physics
{

Fem1DRepresentationLocalization::Fem1DRepresentationLocalization()
{

}

Fem1DRepresentationLocalization::Fem1DRepresentationLocalization(std::shared_ptr<Representation> representation) :
	Localization()
{
	setRepresentation(representation);
}

Fem1DRepresentationLocalization::~Fem1DRepresentationLocalization()
{

}

void Fem1DRepresentationLocalization::setLocalPosition(const SurgSim::DataStructures::IndexedLocalCoordinate& p)
{
	auto femRepresentation = std::static_pointer_cast<Fem1DRepresentation>(getRepresentation());

	SURGSIM_ASSERT(femRepresentation != nullptr) << "FemRepresentation is null, it was probably not" <<
		" initialized";

	SURGSIM_ASSERT(femRepresentation->isValidCoordinate(p))
		<< "IndexedLocalCoordinate is invalid for Representation " << getRepresentation()->getName();

	m_position = p;
}

const SurgSim::DataStructures::IndexedLocalCoordinate& Fem1DRepresentationLocalization::getLocalPosition() const
{
	return m_position;
}

SurgSim::Math::Vector3d Fem1DRepresentationLocalization::doCalculatePosition(double time)
{
	using SurgSim::Math::Vector3d;

	auto femRepresentation = std::static_pointer_cast<Fem1DRepresentation>(getRepresentation());

	SURGSIM_ASSERT(femRepresentation != nullptr) << "FemRepresentation is null, it was probably not" <<
		" initialized";

	std::shared_ptr<FemElement> femElement = femRepresentation->getFemElement(m_position.index);
	const Vector3d currentPosition = femElement->computeCartesianCoordinate(*femRepresentation->getCurrentState(),
																			m_position.coordinate);
	const Vector3d previousPosition = femElement->computeCartesianCoordinate(*femRepresentation->getPreviousState(),
																			 m_position.coordinate);

	if (time == 0.0)
	{
		return previousPosition;
	}
	else if (time == 1.0)
	{
		return currentPosition;
	}

	return previousPosition + time * (currentPosition - previousPosition);
}

bool Fem1DRepresentationLocalization::isValidRepresentation(std::shared_ptr<Representation> representation)
{
	auto femRepresentation = std::dynamic_pointer_cast<Fem1DRepresentation>(representation);

	// Allows to reset the representation to nullptr ...
	return (femRepresentation != nullptr || representation == nullptr);
}

} // namespace Physics

} // namespace SurgSim
