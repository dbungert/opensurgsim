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

#include <memory>

#include <gtest/gtest.h>

#include "SurgSim/Math/OdeState.h"

using SurgSim::Math::OdeState;

namespace
{
	const double epsilon = 1e-10;
};

TEST(OdeStateTest, ConstructorTest)
{
	// Test the constructor normally
	ASSERT_NO_THROW({OdeState state;});

	// Test the object creation through the operator new
	// Eigen needs special care with fixed-size matrix member variables of a class created dynamically via new.
	// We are using non fixed-size matrix, so it should be all fine...this is just to make sure.
	ASSERT_NO_THROW({OdeState* state = new OdeState; delete state; });

	// Test the object creation through the operator new []
	// Eigen needs special care with fixed-size matrix member variables of a class created dynamically via new [].
	// We are using non fixed-size matrix, so it should be all fine...this is just to make sure.
	ASSERT_NO_THROW({OdeState* state = new OdeState[10]; delete [] state; });

	// Test the object creation through std::shared_ptr
	// Eigen needs special care with fixed-size matrix member variables of a class created dynamically via new.
	// We are using non fixed-size matrix, so it should be all fine...this is just to make sure.
	ASSERT_NO_THROW({std::shared_ptr<OdeState> state = std::make_shared<OdeState>();});
}

TEST(OdeStateTest, AllocateTest)
{
	OdeState state;
	EXPECT_EQ(0u, state.getNumDof());
	EXPECT_EQ(0u, state.getNumNodes());
	EXPECT_EQ(0u, state.getNumBoundaryConditions());
	EXPECT_EQ(static_cast<std::vector<unsigned int>::size_type>(0), state.getBoundaryConditions().size());
	EXPECT_EQ(0, state.getPositions().size());
	EXPECT_EQ(0, state.getVelocities().size());

	ASSERT_NO_THROW(state.setNumDof(3u, 3u));
	EXPECT_EQ(9u, state.getNumDof());
	EXPECT_EQ(3u, state.getNumNodes());
	EXPECT_EQ(9, state.getPositions().size());
	EXPECT_EQ(9, state.getVelocities().size());
	EXPECT_EQ(0u , state.getNumBoundaryConditions());
	EXPECT_EQ(static_cast<std::vector<unsigned int>::size_type>(0) , state.getBoundaryConditions().size());
}

TEST(OdeStateTest, GetPositionsTest)
{
	OdeState state1, state2;
	state1.setNumDof(3u, 3u);
	state2.setNumDof(3u, 3u);
	for(unsigned int i = 0; i < state1.getNumDof(); i++)
	{
		state1.getPositions()[i] = static_cast<double>(i);
		state2.getPositions()[i] = 0.0;
	}
	// state1.m_x contains (0 1 2 3 4 5 6 7 8 9 10) & state2.m_x contains (0 0 0 0 0 0 0 0 0 0 0)
	EXPECT_NE(state2.getPositions(), state1.getPositions());
	state2.getPositions() = state1.getPositions();
	// state1.m_x contains (0 1 2 3 4 5 6 7 8 9 10) & state2.m_x contains (0 1 2 3 4 5 6 7 8 9 10)
	EXPECT_EQ(state2.getPositions(), state1.getPositions());

	state1.reset();
	// state1.m_x contains (0 0 0 0 0 0 0 0 0 0 0) & state2.m_x contains (0 1 2 3 4 5 6 7 8 9 10)
	for(unsigned int i = 0; i < state1.getNumDof(); i++)
	{
		EXPECT_EQ(0.0, state1.getPositions()[i]);
		EXPECT_EQ(static_cast<double>(i), state2.getPositions()[i]);
	}

	state2.reset();
	// state1.m_x contains (0 0 0 0 0 0 0 0 0 0 0) & state2.m_x contains (0 0 0 0 0 0 0 0 0 0 0)
	EXPECT_EQ(state2.getPositions(), state1.getPositions());
}

TEST(OdeStateTest, GetVelocitiesTest)
{
	OdeState state1, state2;
	state1.setNumDof(3u, 3u);
	state2.setNumDof(3u, 3u);
	for(unsigned int i = 0; i < state1.getNumDof(); i++)
	{
		state1.getVelocities()[i] = static_cast<double>(i);
		state2.getVelocities()[i] = 0.0;
	}
	// state1.m_v contains (0 1 2 3 4 5 6 7 8 9 10) & state2.m_v contains (0 0 0 0 0 0 0 0 0 0 0)
	EXPECT_NE(state2.getVelocities(), state1.getVelocities());
	state2.getVelocities() = state1.getVelocities();
	// state1.m_v contains (0 1 2 3 4 5 6 7 8 9 10) & state2.m_v contains (0 1 2 3 4 5 6 7 8 9 10)
	EXPECT_EQ(state2.getVelocities(), state1.getVelocities());

	state1.reset();
	// state1.m_v contains (0 0 0 0 0 0 0 0 0 0 0) & state2.m_v contains (0 1 2 3 4 5 6 7 8 9 10)
	for(unsigned int i = 0; i < state1.getNumDof(); i++)
	{
		EXPECT_EQ(0.0, state1.getVelocities()[i]);
		EXPECT_EQ(static_cast<double>(i), state2.getVelocities()[i]);
	}

	state2.reset();
	// state1.m_v contains (0 0 0 0 0 0 0 0 0 0 0) & state2.m_v contains (0 0 0 0 0 0 0 0 0 0 0)
	EXPECT_EQ(state2.getVelocities(), state1.getVelocities());
}

TEST(OdeStateTest, AddGetIsBoundaryConditionsTest)
{
	OdeState state;
	state.setNumDof(3u, 2u);

	state.addBoundaryCondition(0);
	EXPECT_EQ(6u, state.getNumDof());
	EXPECT_EQ(1u, state.getNumBoundaryConditions());
	ASSERT_EQ(static_cast<std::vector<unsigned int>::size_type>(1), state.getBoundaryConditions().size());
	for (unsigned int dofId = 1; dofId < 6; dofId++)
	{
		if (dofId == 0u)
		{
			EXPECT_TRUE(state.isBoundaryCondition(dofId));
		}
		else
		{
			EXPECT_FALSE(state.isBoundaryCondition(dofId));
		}
	}
	EXPECT_EQ(0u, state.getBoundaryConditions()[0]);
	EXPECT_EQ(6, state.getPositions().size());
	EXPECT_EQ(6, state.getVelocities().size());
	EXPECT_TRUE(state.getPositions().isZero());
	EXPECT_TRUE(state.getVelocities().isZero());

	state.addBoundaryCondition(2);
	EXPECT_EQ(6u, state.getNumDof());
	EXPECT_EQ(2u, state.getNumBoundaryConditions());
	ASSERT_EQ(static_cast<std::vector<unsigned int>::size_type>(2), state.getBoundaryConditions().size());
	EXPECT_EQ(0u, state.getBoundaryConditions()[0]);
	EXPECT_EQ(2u, state.getBoundaryConditions()[1]);
	for (unsigned int dofId = 1; dofId < 6; dofId++)
	{
		if (dofId == 0u || dofId == 2u)
		{
			EXPECT_TRUE(state.isBoundaryCondition(dofId));
		}
		else
		{
			EXPECT_FALSE(state.isBoundaryCondition(dofId));
		}
	}
	EXPECT_EQ(6, state.getPositions().size());
	EXPECT_EQ(6, state.getVelocities().size());
	EXPECT_TRUE(state.getPositions().isZero());
	EXPECT_TRUE(state.getVelocities().isZero());

	state.addBoundaryCondition(4);
	EXPECT_EQ(6u, state.getNumDof());
	EXPECT_EQ(3u, state.getNumBoundaryConditions());
	ASSERT_EQ(static_cast<std::vector<unsigned int>::size_type>(3), state.getBoundaryConditions().size());
	EXPECT_EQ(0u, state.getBoundaryConditions()[0]);
	EXPECT_EQ(2u, state.getBoundaryConditions()[1]);
	EXPECT_EQ(4u, state.getBoundaryConditions()[2]);
	for (unsigned int dofId = 1; dofId < 6; dofId++)
	{
		if (dofId == 0u || dofId == 2u || dofId == 4u)
		{
			EXPECT_TRUE(state.isBoundaryCondition(dofId));
		}
		else
		{
			EXPECT_FALSE(state.isBoundaryCondition(dofId));
		}
	}
	EXPECT_EQ(6, state.getPositions().size());
	EXPECT_EQ(6, state.getVelocities().size());
	EXPECT_TRUE(state.getPositions().isZero());
	EXPECT_TRUE(state.getVelocities().isZero());
}

TEST(OdeStateTest, ResetTest)
{
	OdeState state1, state2;
	state1.setNumDof(3u, 3u);
	state2.setNumDof(3u, 3u);
	for(unsigned int i = 0; i < state1.getNumDof(); i++)
	{
		state1.getPositions()[i] = static_cast<double>(i);
		state1.getVelocities()[i] = 2.0*static_cast<double>(i);
	}
	state1.addBoundaryCondition(0);
	state1.addBoundaryCondition(state1.getNumDof() - 1);
	EXPECT_NE(state2, state1);

	state1.reset();
	EXPECT_EQ(state2, state1);
	EXPECT_EQ(9u, state1.getNumDof());
	EXPECT_EQ(3u, state1.getNumNodes());
	EXPECT_TRUE(state1.getPositions().isZero());
	EXPECT_TRUE(state1.getVelocities().isZero());
	EXPECT_EQ(0u, state1.getNumBoundaryConditions());
	EXPECT_EQ(static_cast<std::vector<unsigned int>::size_type>(0), state1.getBoundaryConditions().size());
}

TEST(OdeStateTest, CopyConstructorAndAssignmentTest)
{
	OdeState state, stateAssigned;
	state.setNumDof(3u, 3u);
	for(unsigned int i = 0; i < state.getNumDof(); i++)
	{
		state.getPositions()[i] = static_cast<double>(i);
		state.getVelocities()[i] = 2.0*static_cast<double>(i);
	}
	state.addBoundaryCondition(0);
	state.addBoundaryCondition(state.getNumDof() - 1);

	{
		OdeState stateCopied(state);

		ASSERT_EQ(9u, stateCopied.getNumDof());
		ASSERT_EQ(state.getNumDof(), stateCopied.getNumDof());
		ASSERT_EQ(9, stateCopied.getPositions().size());
		ASSERT_EQ(state.getPositions().size(), stateCopied.getPositions().size());
		ASSERT_EQ(9, stateCopied.getVelocities().size());
		ASSERT_EQ(state.getVelocities().size(), stateCopied.getVelocities().size());

		for(unsigned int i = 0; i < stateCopied.getNumDof(); i++)
		{
			EXPECT_NEAR(state.getPositions()[i], stateCopied.getPositions()[i], epsilon);
			EXPECT_NEAR(static_cast<double>(i), stateCopied.getPositions()[i], epsilon);
			EXPECT_NEAR(state.getVelocities()[i], stateCopied.getVelocities()[i], epsilon);
			EXPECT_NEAR(2.0*static_cast<double>(i), stateCopied.getVelocities()[i], epsilon);
		}

		ASSERT_EQ(2u, stateCopied.getNumBoundaryConditions());
		ASSERT_EQ(state.getNumBoundaryConditions(), stateCopied.getNumBoundaryConditions());
		ASSERT_EQ(static_cast<std::vector<unsigned int>::size_type>(2), stateCopied.getBoundaryConditions().size());
		ASSERT_EQ(state.getBoundaryConditions().size(), stateCopied.getBoundaryConditions().size());
		ASSERT_EQ(0u, stateCopied.getBoundaryConditions()[0]);
		ASSERT_EQ(state.getBoundaryConditions()[0], stateCopied.getBoundaryConditions()[0]);
		ASSERT_EQ(state.getNumDof() - 1, stateCopied.getBoundaryConditions()[1]);
		ASSERT_EQ(state.getBoundaryConditions()[1], stateCopied.getBoundaryConditions()[1]);
	}

	{
		stateAssigned = state;

		ASSERT_EQ(9u, stateAssigned.getNumDof());
		ASSERT_EQ(state.getNumDof(), stateAssigned.getNumDof());
		ASSERT_EQ(9, stateAssigned.getPositions().size());
		ASSERT_EQ(state.getPositions().size(), stateAssigned.getPositions().size());
		ASSERT_EQ(9, stateAssigned.getVelocities().size());
		ASSERT_EQ(state.getVelocities().size(), stateAssigned.getVelocities().size());

		for(unsigned int i = 0; i < stateAssigned.getNumDof(); i++)
		{
			EXPECT_NEAR(state.getPositions()[i], stateAssigned.getPositions()[i], epsilon);
			EXPECT_NEAR(static_cast<double>(i), stateAssigned.getPositions()[i], epsilon);
			EXPECT_NEAR(state.getVelocities()[i], stateAssigned.getVelocities()[i], epsilon);
			EXPECT_NEAR(2.0*static_cast<double>(i), stateAssigned.getVelocities()[i], epsilon);
		}

		ASSERT_EQ(2u, stateAssigned.getNumBoundaryConditions());
		ASSERT_EQ(state.getNumBoundaryConditions(), stateAssigned.getNumBoundaryConditions());
		ASSERT_EQ(static_cast<std::vector<unsigned int>::size_type>(2), stateAssigned.getBoundaryConditions().size());
		ASSERT_EQ(state.getBoundaryConditions().size(), stateAssigned.getBoundaryConditions().size());
		ASSERT_EQ(0u, stateAssigned.getBoundaryConditions()[0]);
		ASSERT_EQ(state.getBoundaryConditions()[0], stateAssigned.getBoundaryConditions()[0]);
		ASSERT_EQ(state.getNumDof() - 1, stateAssigned.getBoundaryConditions()[1]);
		ASSERT_EQ(state.getBoundaryConditions()[1], stateAssigned.getBoundaryConditions()[1]);
	}
}

TEST(OdeStateTest, ApplyBoundaryConditionsToVectorTest)
{
	OdeState state;
	state.setNumDof(3u, 3u);
	state.addBoundaryCondition(1u);
	state.addBoundaryCondition(state.getNumDof() - 1u);

	SurgSim::Math::Vector F(state.getNumDof());
	F.setLinSpaced(1.0, 2.0);
	SurgSim::Math::Vector initialF = F;

	state.applyBoundaryConditionsToVector(&F);
	EXPECT_FALSE(F.isApprox(initialF));
	for (unsigned int dofId = 0; dofId < state.getNumDof(); ++dofId)
	{
		if (dofId == 1u || dofId == state.getNumDof() - 1u)
		{
			EXPECT_NE(initialF[dofId], F[dofId]);
			EXPECT_EQ(0u, F[dofId]);
		}
		else
		{
			EXPECT_EQ(initialF[dofId], F[dofId]);
		}
	}
}

TEST(OdeStateTest, ApplyBoundaryConditionsToMatrixTest)
{
	OdeState state;
	state.setNumDof(3u, 3u);
	state.addBoundaryCondition(1u);
	state.addBoundaryCondition(state.getNumDof() - 1u);

	unsigned int numDof = state.getNumDof();
	SurgSim::Math::Matrix M = 2.0 * SurgSim::Math::Matrix::Ones(numDof, numDof);
	SurgSim::Math::Matrix initialM = M;
	SurgSim::Math::Matrix expectedM = M;
	for (int bcId = 0; bcId < 2; ++bcId)
	{
		unsigned int dofId = state.getBoundaryConditions()[bcId];
		expectedM.block<1, 9>(dofId, 0).setZero();
		expectedM.block<9, 1>(0, dofId).setZero();
		expectedM(dofId, dofId) = 1.0;
	}

	state.applyBoundaryConditionsToMatrix(&M);
	EXPECT_FALSE(M.isApprox(initialM));
	EXPECT_TRUE(M.isApprox(expectedM));
}

namespace
{
void testIsValidWith(double invalidNumber)
{
	OdeState invalidStateInfinityOnPosition;
	invalidStateInfinityOnPosition.setNumDof(3u, 3u);
	invalidStateInfinityOnPosition.getPositions().setOnes();
	invalidStateInfinityOnPosition.getPositions()[2] = invalidNumber;
	EXPECT_FALSE(invalidStateInfinityOnPosition.isValid());

	OdeState invalidStateInfinityOnVelocity;
	invalidStateInfinityOnVelocity.setNumDof(3u, 3u);
	invalidStateInfinityOnVelocity.getVelocities().setOnes();
	invalidStateInfinityOnVelocity.getVelocities()[2] = invalidNumber;
	EXPECT_FALSE(invalidStateInfinityOnVelocity.isValid());
}
}; // anonymous namespace

TEST(OdeStateTest, IsValidTest)
{
	OdeState validState;
	validState.setNumDof(3u, 3u);
	validState.getPositions().setOnes();
	EXPECT_TRUE(validState.isValid());

	{
		SCOPED_TRACE("Test with invalid INF");
		testIsValidWith(std::numeric_limits<double>::infinity());
	}

	{
		SCOPED_TRACE("Test with invalid QuietNaN");
		testIsValidWith(std::numeric_limits<double>::quiet_NaN());
	}

	{
		SCOPED_TRACE("Test with invalid SignalingNaN");
		testIsValidWith(std::numeric_limits<double>::signaling_NaN());
	}
}
