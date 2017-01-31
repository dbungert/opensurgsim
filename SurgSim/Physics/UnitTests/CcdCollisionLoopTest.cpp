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

#include <gtest/gtest.h>

#include "SurgSim/Collision/CollisionPair.h"
#include "SurgSim/DataStructures/Location.h"
#include "SurgSim/Math/Vector.h"
#include "SurgSim/Physics/CcdCollisionLoop.h"
#include "SurgSim/Physics/PushResults.h"
#include "SurgSim/Physics/RigidCollisionRepresentation.h"
#include "SurgSim/Physics/SolveMlcp.h"

namespace SurgSim
{
namespace Physics
{

TEST(CcdCollisionLoopTest, Setters)
{
	auto ccd = std::make_shared<CcdCollisionLoop>(false);
	std::unique_ptr<SolveMlcp> solveMlcp(new SolveMlcp(false));
	ccd->setSolveMlcp(std::move(solveMlcp));

	std::unique_ptr<PushResults> pushResults(new PushResults(false));
	ccd->setPushResults(std::move(pushResults));
}

TEST(CcdCollisionLoopTest, FilterContacts)
{
	double toi;
	auto rep1 = std::make_shared<RigidCollisionRepresentation>("One");
	rep1->setSelfCollisionDetectionType(Collision::COLLISION_DETECTION_TYPE_CONTINUOUS);
	auto pair = std::make_shared<Collision::CollisionPair>(rep1, rep1);

	auto computation = std::make_shared<CcdCollisionLoop>(false);
	DataStructures::Location location;
	std::vector<std::shared_ptr<Collision::CollisionPair>> pairs(1, pair);

	EXPECT_FALSE(computation->findEarliestContact(pairs, &toi));

	// Check that we find the toi correctly
	pair->addCcdContact(0.0, 0.1, Math::Vector3d::Zero(), Math::Vector3d::Zero(),
						std::make_pair(location, location));
	EXPECT_TRUE(computation->findEarliestContact(pairs, &toi));
	EXPECT_DOUBLE_EQ(0.1, toi);
	computation->filterLaterContacts(&pairs, 0.0, toi);
	EXPECT_EQ(1u, pair->getContacts().size());

	pair->addCcdContact(0.0, 0.2, Math::Vector3d::Zero(), Math::Vector3d::Zero(),
						std::make_pair(location, location));

	pair->addCcdContact(0.0, 0.3, Math::Vector3d::Zero(), Math::Vector3d::Zero(),
						std::make_pair(location, location));

	// Check that we filter everything after the toi
	toi = 0.0;
	EXPECT_EQ(3u, pair->getContacts().size());
	EXPECT_TRUE(computation->findEarliestContact(pairs, &toi));
	computation->filterLaterContacts(&pairs, 0.0, toi);
	EXPECT_DOUBLE_EQ(0.1, toi);
	EXPECT_EQ(1u, pair->getContacts().size());
}

TEST(CcdCollisionLoopTest, FilterContactsWithEpsilon)
{
	double toi;
	auto rep1 = std::make_shared<RigidCollisionRepresentation>("One");
	rep1->setSelfCollisionDetectionType(Collision::COLLISION_DETECTION_TYPE_CONTINUOUS);
	auto pair = std::make_shared<Collision::CollisionPair>(rep1, rep1);

	auto computation = std::make_shared<CcdCollisionLoop>(false);
	DataStructures::Location location;
	std::vector<std::shared_ptr<Collision::CollisionPair>> pairs(1, pair);

	// Check that we find the toi correctly
	pair->addCcdContact(0.0, 0.1, Math::Vector3d::Zero(), Math::Vector3d::Zero(),
						std::make_pair(location, location));
	pair->addCcdContact(0.0, 0.2, Math::Vector3d::Zero(), Math::Vector3d::Zero(),
						std::make_pair(location, location));
	pair->addCcdContact(0.0, 0.3, Math::Vector3d::Zero(), Math::Vector3d::Zero(),
						std::make_pair(location, location));
	EXPECT_TRUE(computation->findEarliestContact(pairs, &toi));
	computation->filterLaterContacts(&pairs, 0.11, toi);
	// toi should be 0.1 + 0.11 i.e. toi + epsilon
	EXPECT_DOUBLE_EQ(0.1, toi);
	EXPECT_EQ(2u, pair->getContacts().size());
}

}
}
