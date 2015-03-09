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

#ifndef SURGSIM_COLLISION_OCTREECAPSULEDCDCONTACT_H
#define SURGSIM_COLLISION_OCTREECAPSULEDCDCONTACT_H

#include "SurgSim/Collision/BoxCapsuleDcdContact.h"
#include "SurgSim/Collision/OctreeDcdContact.h"

namespace SurgSim
{
namespace Collision
{

class OctreeCapsuleDcdContact : public OctreeDcdContact
{
public:
	std::pair<int, int> getShapeTypes() override;

protected:
	std::list<std::shared_ptr<Contact>> boxContactCalculation(const SurgSim::Math::BoxShape& boxShape,
			const SurgSim::Math::RigidTransform3d& boxPose, const SurgSim::Math::Shape& otherShape,
			const SurgSim::Math::RigidTransform3d& otherPose) override;

private:
	BoxCapsuleDcdContact m_calculator;
};

};
};

#endif //SURGSIM_COLLISION_OCTREECAPSULEDCDCONTACT_H


