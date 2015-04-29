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

#ifndef SURGSIM_PHYSICS_FEMELEMENT1DMESHPLYREADERDELEGATE_H
#define SURGSIM_PHYSICS_FEMELEMENT1DMESHPLYREADERDELEGATE_H

#include <array>
#include <memory>

#include "SurgSim/DataStructures/EmptyData.h"
#include "SurgSim/DataStructures/PlyReader.h"
#include "SurgSim/DataStructures/TriangleMeshPlyReaderDelegate.h"
#include "SurgSim/Math/OdeState.h"
#include "SurgSim/Physics/FemElementMesh.h"

using SurgSim::DataStructures::EmptyData;
using SurgSim::DataStructures::PlyReader;
using SurgSim::DataStructures::PlyReaderDelegate;

namespace SurgSim
{
namespace Physics
{

class FemElement1DMeshPlyReaderDelegate : public PlyReaderDelegate
{
public:
	/// Default constructor.
	FemElement1DMeshPlyReaderDelegate();

	/// Constructor.
	/// \param mesh The mesh to be used, it will be cleared by the constructor.
	explicit FemElement1DMeshPlyReaderDelegate(std::shared_ptr<FemElement1DMesh> mesh);

	/// Registers the delegate with the reader, overridden from \sa PlyReaderDelegate.
	/// \param reader The reader that should be used.
	/// \return true if it succeeds, false otherwise.
	bool registerDelegate(PlyReader* reader);

	/// Check whether this file is acceptable to the delegate, overridden from \sa PlyReaderDelegate.
	/// \param reader The reader that should be used.
	/// \return true if it succeeds, false otherwise.
	bool fileIsAcceptable(const PlyReader& reader);

	/// Callback function, begin the processing of vertices.
	/// \param elementName Name of the element.
	/// \param vertexCount Number of vertices.
	/// \return memory for vertex data to the reader.
	void* beginVertices(const std::string& elementName, size_t vertexCount);

	/// Callback function to process one vertex.
	/// \param elementName Name of the element.
	void processVertex(const std::string& elementName);

	/// Callback function to finalize processing of vertices.
	/// \param elementName Name of the element.
	void endVertices(const std::string& elementName);

	/// Callback function, begin the processing of materials.
	/// \param elementName Name of the element.
	/// \param materialCount Number of materials.
	/// \return memory for material data to the reader.
	void* beginMaterials(const std::string& elementName, size_t materialCount);

	/// Callback function, end the processing of materials.
	/// \param elementName Name of the element.
	void endMaterials(const std::string& elementName);

	/// Callback function, begin the processing of radius.
	/// \param elementName Name of the element.
	/// \param radiusCount Number of radii.
	/// \return memory for radius data to the reader.
	void* beginRadius(const std::string& elementName, size_t radiusCount);

	/// Callback function, end the processing of radius.
	/// \param elementName Name of the element.
	void endRadius(const std::string& elementName);

	/// Callback function, begin the processing of boundary conditions.
	/// \param elementName Name of the element.
	/// \param boundaryConditionCount Number of boundary conditions.
	/// \return memory for boundary conditions data to the reader.
	void* beginBoundaryConditions(const std::string& elementName, size_t boundaryConditionCount);

	/// Callback function to process one boundary condition.
	/// \param elementName Name of the element.
	void processBoundaryCondition(const std::string& elementName);

	void endFile();

private:
	struct FemElement1D
	{
		unsigned int type;   // “LinearBeam”, “CorotationalTetrahedron”…
		int64_t overrun1; ///< Used to check for buffer overruns

		std::vector<size_t> nodeIds;
		int64_t overrun2; ///< Used to check for buffer overruns
	} m_elementData;

	struct Material
	{
		double youngModulus;
		double poissonRatio;
		double massDensity;
		int64_t overrun; ///< Used to check for buffer overruns
	} m_materialData;

	struct Vertex6DData
	{
		double x;
		double y;
		double z;
		int64_t overrun1; ///< Used to check for buffer overruns
		double thetaX;
		double thetaY;
		double thetaZ;
		int64_t overrun2; ///< Used to check for buffer overruns
	} m_vertexData;

	bool m_hasRotationDOF;

	double m_radius;
	bool m_enableShear;

	/// Flag indicating if the associated file has boundary conditions
	bool m_hasBoundaryConditions;

	/// Internal data to receive the "boundary_condition" element
	size_t m_boundaryConditionData;

	std::shared_ptr<FemElement1DMesh> m_mesh;

	/// The state that will be created by loading
	std::shared_ptr<SurgSim::Math::OdeState> m_state;
};

} // namespace Physics
} // namespace SurgSim

#endif // SURGSIM_PHYSICS_FEMELEMENT1DMESHPLYREADERDELEGATE_H
