// This file is a part of the OpenSurgSim project.
// Copyright 2015, SimQuest Solutions Inc.
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

#ifndef SURGSIM_PHYSICS_MASSSPRING_H
#define SURGSIM_PHYSICS_MASSSPRING_H

#include <vector>

#include "SurgSim/DataStructures/Vertices.h"
#include "SurgSim/Framework/Asset.h"
#include "SurgSim/Physics/Mass.h"

namespace SurgSim
{
namespace Physics
{

class Spring;

SURGSIM_STATIC_REGISTRATION(MassSpring);

/// Base class for a data structure for holding MassSpring mesh data.
///
/// MassSpring itself should not be used directly itself as it contains no override for doLoad since the implementation
/// is dependent on the dimension of the MassSpring you are trying to load. Each dimension overrides the doLoad function
/// present in Asset using its own version of an MassSpringPlyReaderDelegate.
class MassSpring : public SurgSim::DataStructures::Vertices<Mass>, public SurgSim::Framework::Asset,
		public std::enable_shared_from_this<MassSpring>
{
public:
	/// Default constructor
	MassSpring();

	SURGSIM_CLASSNAME(SurgSim::Physics::MassSpring);

	/// Adds a mass
	/// \param mass The mass to add to the representation
	/// \note Masses are kept in an ordered list, giving them an index
	/// \note This mass will be associated with the node of same index in any associated OdeState
	void addMass(const std::shared_ptr<Mass> mass);

	/// Adds a spring
	/// \param spring The spring to add to the representation
	void addSpring(const std::shared_ptr<Spring> spring);

	/// Gets the number of masses
	/// \return the number of masses
	size_t getNumMasses() const;

	/// Gets the number of springs
	/// \return the number of springs
	size_t getNumSprings() const;

	/// Retrieves the masses.
	const std::vector<std::shared_ptr<Mass>>& getMasses() const;

	/// Retrieves a given spring from its id
	/// \param springId The spring id for which the spring is requested
	/// \return the spring for the given springId
	/// \note The spring is returned with read/write access
	/// \note Out of range springId will raise an exception
	const std::vector<std::shared_ptr<Spring>>& getSprings() const;

	/// Retrieves the mass of a given node
	/// \param nodeId The node id for which the mass is requested
	/// \return the mass attribute of a node
	/// \note The mass is returned with read/write access
	/// \note Out of range nodeId will raise an exception
	std::shared_ptr<Mass> getMass(size_t nodeId);

	/// Retrieves a given spring from its id
	/// \param springId The spring id for which the spring is requested
	/// \return the spring for the given springId
	/// \note The spring is returned with read/write access
	/// \note Out of range springId will raise an exception
	std::shared_ptr<Spring> getSpring(size_t springId);

	/// Add boundary condition to mesh
	/// \param boundaryCondition A vertex id that has a boundary condition
	/// \return The new size of the vector of boundary conditions
	size_t addBoundaryCondition(size_t boundaryCondition);

	/// Gets entire vector of boundary conditions
	/// \return A vector of boundary conditions
	const std::vector<size_t>& getBoundaryConditions() const;

	/// Gets entire vector of boundary conditions (non-const)
	/// \return A vector of boundary conditions
	std::vector<size_t>& getBoundaryConditions();

	/// Retrieves a specific boundary condition
	/// \param id The id of the boundary condition in the vector
	/// \return The vertex id which has a boundary condition
	size_t getBoundaryCondition(size_t id) const;

protected:
	bool doLoad(const std::string& filePath) override;

	/// Masses
	std::vector<std::shared_ptr<Mass>> m_masses;

	/// Springs
	std::vector<std::shared_ptr<Spring>> m_springs;

	/// Vector of vertex ids that have boundary conditions
	std::vector<size_t> m_boundaryConditions;
};

} // namespace Physics
} // namespace SurgSim

#endif // SURGSIM_PHYSICS_MASSSPRING_H
