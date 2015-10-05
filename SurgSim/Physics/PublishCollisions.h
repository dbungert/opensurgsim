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

#ifndef SURGSIM_PHYSICS_PUBLISHCOLLISIONS_H
#define SURGSIM_PHYSICS_PUBLISHCOLLISIONS_H

#include <memory>

#include "SurgSim/Physics/Computation.h"

namespace SurgSim
{
namespace Physics
{
class PhysicsManagerState;

/// Publish the collision list for each representation
class PublishCollisions : public Computation
{
public:
	/// Constructor
	/// \param doCopyState Specify if the output state in Computation::Update() is a copy or not of the input state
	explicit PublishCollisions(bool doCopyState = false);

	/// Destructor
	virtual ~PublishCollisions();

protected:
	std::shared_ptr<PhysicsManagerState> doUpdate(const double& dt, const std::shared_ptr<PhysicsManagerState>& state)
		override;
};

}; // Physics
}; // SurgSim

#endif // SURGSIM_PHYSICS_PUBLISHCOLLISIONS_H
