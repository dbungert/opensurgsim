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

#include <SurgSim/Blocks/TransferDeformableStateToVerticesBehavior.h>

namespace SurgSim
{

namespace Blocks
{

template <>
void TransferDeformableStateToVerticesBehavior<void>::transfer(bool doInitialization)
{
	const unsigned int numNodes = m_from->getNumNodes();

	// If initialization is requested and vertices is empty, let's populate it properly
	if (doInitialization == true && m_to->getNumVertices() == 0 && numNodes != 0)
	{
		for (unsigned int nodeId = 0; nodeId < numNodes; nodeId++)
		{
			SurgSim::DataStructures::Vertex<void> v(m_from->getPosition(nodeId));
			m_to->addVertex(v);
		}
	}
	else if (m_to->getNumVertices() == numNodes)
	{
		for (unsigned int nodeId = 0; nodeId < numNodes; nodeId++)
		{
			m_to->setVertexPosition(nodeId, m_from->getPosition(nodeId));
		}
	}
}

}; //namespace Blocks

}; //namespace SurgSim
