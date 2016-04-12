// This file is a part of the OpenSurgSim project.
// Copyright 2013-2016, SimQuest Solutions Inc.
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

#include "SurgSim/Physics/CcdCollisionLoop.h"

#include "SurgSim/Physics/CcdCollision.h"
#include "SurgSim/Physics/UpdateCcdData.h"
#include "SurgSim/Physics/ContactConstraintGeneration.h"
#include "SurgSim/Physics/SolveMlcp.h"
#include "SurgSim/Physics/PushResults.h"
#include "SurgSim/Physics/BuildMlcp.h"
#include "SurgSim/Physics/PhysicsManagerState.h"
#include "SurgSim/Collision/CollisionPair.h"

#include <unordered_set>
#include <limits>

namespace SurgSim
{

namespace Physics
{

CcdCollisionLoop::CcdCollisionLoop(bool copyState) :
	Computation(copyState),
	m_updateCcdData(new UpdateCcdData(copyState)),
	m_ccdCollision(new CcdCollision(copyState)),
	m_constraintGeneration(new ContactConstraintGeneration(copyState)),
	m_buildMlcp(new BuildMlcp(copyState)),
	m_solveMlcp(new SolveMlcp(copyState)),
	m_pushResults(new PushResults(copyState)),
	m_maxIterations(20),
	m_epsilonFactor(100),
	m_logger(SurgSim::Framework::Logger::getLogger("Physics/CCDCollisionLoop"))
{
}

CcdCollisionLoop::~CcdCollisionLoop()
{

}

std::shared_ptr<SurgSim::Physics::PhysicsManagerState> CcdCollisionLoop::doUpdate(const double& dt,
		const std::shared_ptr<PhysicsManagerState>& state)
{
	auto lastState = state;
	size_t iterations = m_maxIterations;

	auto collisionPairs = state->getCollisionPairs();
	std::vector<std::shared_ptr<Collision::CollisionPair>> ccdPairs;
	ccdPairs.reserve(collisionPairs.size());

	std::copy_if(collisionPairs.cbegin(), collisionPairs.cend(), std::back_inserter(ccdPairs),
				 [](const std::shared_ptr<Collision::CollisionPair>& p)
	{
		if (p->getType() == Collision::COLLISION_DETECTION_TYPE_CONTINUOUS)
		{
			return true;
		}
		return false;
	});

	// localDt is the time left on dt as we step through the CCD, this gets reduced as contacts are found
	double localDt = dt;
	double toi = 0.0;
	double localToi = 0.0;

	while (--iterations > 0)
	{
		toi += (1.0 - toi) * localToi;
		double epsilon = 1.0 / ((1 - toi) * m_epsilonFactor);

		lastState = m_updateCcdData->update(localToi, lastState); // state interpolation is triggered in here
		lastState = m_ccdCollision->update(dt, lastState);

		if (m_logger->getThreshold() <= SurgSim::Framework::LOG_LEVEL_DEBUG)
		{
			printContacts(ccdPairs);
		}

		// Find the first impact and filter all contacts beyond a given epsilon
		if (!filterContacts(ccdPairs, epsilon, &localToi))
		{
			break;
		}

		lastState = m_constraintGeneration->update(dt, lastState);
		lastState = m_buildMlcp->update(dt, lastState);
		lastState = m_solveMlcp->update(dt, lastState);
		lastState = m_pushResults->update(dt, lastState);

		clearContacts(ccdPairs);

		if (toi > 1.0)
		{
			break;
		}
	}

	SURGSIM_LOG_IF(iterations == 0, m_logger, WARNING) << "Maxed out iterations (" << m_maxIterations << ")";

	return lastState;
}

bool CcdCollisionLoop::filterContacts(
	const std::vector<std::shared_ptr<Collision::CollisionPair>>& ccdPairs,
	double epsilon,
	double* currentToi)
{
	SURGSIM_ASSERT(currentToi != nullptr);

	double toi = std::numeric_limits<double>::max();

	// Find earliest time of impact
	for (const auto& pair : ccdPairs)
	{
		std::for_each(pair->getContacts().begin(),
					  pair->getContacts().end(), [&toi](const std::shared_ptr<Collision::Contact>& contact)
		{
			toi = std::min<double>(toi, contact->time);
		});
	}

	// Did not find any contacts return false
	if (!(toi < std::numeric_limits<double>::max()))
	{
		return false;
	}

	toi += epsilon;

	for (const auto& pair : ccdPairs)
	{
		pair->getContacts().remove_if([toi](const std::shared_ptr<Collision::Contact>& contact)
		{
			return contact->time > toi;
		});
	}

	*currentToi = toi;

	return true;
}

void CcdCollisionLoop::printContacts(std::vector<std::shared_ptr<Collision::CollisionPair>> ccdPairs)
{
	std::stringstream out;
	size_t contactCount = 0;
	for (const auto& pair : ccdPairs)
	{
		for (const auto& contact : pair->getContacts())
		{
			out << *contact;
			contactCount++;
		}
	}
	SURGSIM_LOG_IF(contactCount != 0, m_logger, DEBUG) << "Contacts: " << contactCount << std::endl << out.str();
}

void CcdCollisionLoop::clearContacts(std::vector<std::shared_ptr<Collision::CollisionPair>> ccdPairs)
{
	for (const auto& pair : ccdPairs)
	{
		pair->getContacts().clear();
	}
}

}
}