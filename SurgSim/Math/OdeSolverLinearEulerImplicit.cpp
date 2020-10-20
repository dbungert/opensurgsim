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

#include "SurgSim/Framework/Log.h"
#include "SurgSim/Math/OdeSolverLinearEulerImplicit.h"
#include "SurgSim/Math/OdeState.h"

namespace SurgSim
{

namespace Math
{

OdeSolverLinearEulerImplicit::OdeSolverLinearEulerImplicit(OdeEquation* equation)
	: OdeSolverEulerImplicit(equation), m_initialized(false)
{
	m_name = "Ode Solver Linear Euler Implicit";

	// The system being linear, only 1 iteration is necessary to find the exact solution.
	setNewtonRaphsonMaximumIteration(1);
}

void OdeSolverLinearEulerImplicit::setNewtonRaphsonMaximumIteration(size_t maximumIteration)
{
	OdeSolverEulerImplicit::setNewtonRaphsonMaximumIteration(maximumIteration);
	SURGSIM_LOG_IF(maximumIteration != 1, SurgSim::Framework::Logger::getLogger("OdeSolver"), WARNING) <<
			"OdeSolverLinearEulerImplicit should have a maximum number of iteration of 1 for the Newton-Raphson. " <<
			"As the model is (supposed to be) linear, a single iteration will find the exact solution.";
}

void OdeSolverLinearEulerImplicit::solve(double dt, const OdeState& currentState, OdeState* newState,
		bool computeCompliance)
{
	if (!m_initialized)
	{
		// The compliance matrix is constant and used in all following calls, so we force its calculation on 1st pass.
		OdeSolverEulerImplicit::solve(dt, currentState, newState, true);
		m_constantK = m_equation.getK().pruned();
		m_initialized = true;
	}
	else
	{
		m_equation.updateFMDK(currentState, ODEEQUATIONUPDATE_F);

		Vector f = m_equation.getF();
		f -= m_constantK * (currentState.getVelocities() * dt);
		Vector deltaV = m_equation.applyCompliance(currentState, f);

		newState->getVelocities() = currentState.getVelocities() + deltaV;
		newState->getPositions()  = currentState.getPositions()  + dt * newState->getVelocities();
	}
}

}; // namespace Math

}; // namespace SurgSim
