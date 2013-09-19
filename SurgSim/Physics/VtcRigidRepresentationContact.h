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

#ifndef SURGSIM_PHYSICS_VTCRIGIDREPRESENTATIONCONTACT_H
#define SURGSIM_PHYSICS_VTCRIGIDREPRESENTATIONCONTACT_H

#include <SurgSim/Physics/Constraint.h>
#include <SurgSim/Physics/ConstraintData.h>
#include <SurgSim/Physics/ConstraintImplementation.h>
#include <SurgSim/Physics/Localization.h>

namespace SurgSim
{

namespace Physics
{

/// VtcRigidRepresentation frictionless contact implementation.
class VtcRigidRepresentationContact : public ConstraintImplementation
{
public:
	/// Constructor
	VtcRigidRepresentationContact();
	/// Destructor
	virtual ~VtcRigidRepresentationContact();

	/// Gets the Mixed Linear Complementarity Problem constraint type for this ConstraintImplementation
	/// \return The MLCP constraint type corresponding to this constraint implementation
	/// Overrides ConstraintImplementation::getMlcpConstraintType()
	virtual SurgSim::Math::MlcpConstraintType getMlcpConstraintType() const override;
	/// Gets the Type of representation that this implementation is concerned with
	/// \return RepresentationType for this implementation
	/// Overrides ConstraintImplementation::getRepresentationType()
	virtual RepresentationType getRepresentationType() const override;

private:
	/// Gets the number of degree of freedom for a frictionless contact.
	/// \return 1 as a frictionless contact only has 1 equation of constraint (along the normal direction).
	/// Overrides ConstraintImplementation::doGetNumDof()
	virtual unsigned int doGetNumDof() const override;

	/// Builds the subset of an Mlcp physics problem associated to this implementation.
	/// \param dt The time step.
	/// \param data The data associated to the constraint.
	/// \param localization The localization for the representation.
	/// \param [in, out] mlcp The Mixed LCP physics problem to fill up.
	/// \param indexOfRepresentation The index of the representation (associated to this implementation) in the mlcp.
	/// \param indexOfConstraint The index of the constraint in the mlcp.
	/// \param sign The sign of this implementation in the constraint (positive or negative side).
	/// Overrides ConstraintImplementation::doBuild()
	virtual void doBuild(double dt,
		const ConstraintData& data,
		const std::shared_ptr<Localization>& localization,
		MlcpPhysicsProblem* mlcp,
		unsigned int indexOfRepresentation,
		unsigned int indexOfConstraint,
		ConstraintSideSign sign) override;
};

};  // namespace Physics

};  // namespace SurgSim

#endif  // SURGSIM_PHYSICS_VTCRIGIDREPRESENTATIONCONTACT_H
